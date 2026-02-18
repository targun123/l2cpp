// Project includes
#include "Packet.hpp"
#include "Packets.hpp"

#include <l2cpp/Blowfish.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party includes
#include <boost/asio.hpp>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <spdlog/spdlog.h>

using boost::asio::ip::tcp;

struct Connection;
using PacketHandler = void (*)(Connection &);

constexpr byte gBlowfishToken[] = "_;5.]94-31==-%xT!^[$"; // trailing zero is included by sizeof()

struct Connection
{
    tcp::socket     socket;
    l2cpp::Blowfish blowfish;

    std::unique_ptr<BIGNUM, decltype(&BN_free)>  bigNum;
    std::unique_ptr<RSA,    decltype(&RSA_free)> rsaKey;

    std::vector<byte> readBuffer;

    std::string userName;
    std::string password;

    explicit Connection(tcp::socket socket)
        : socket(std::move(socket))
        , blowfish(gBlowfishToken)
        , bigNum(nullptr, &BN_free)
        , rsaKey(RSA_new(), &RSA_free)
    {
        BIGNUM * n = nullptr;
        BN_dec2bn(&n, "65537");
        bigNum.reset(n);

        RSA_generate_key_ex(rsaKey.get(), 1024, bigNum.get(), nullptr);
        readBuffer.resize(sizeof(u16) + sizeof(u8) + RSA_size(rsaKey.get())); // size + type + rsa
    }

    void send(Packet & p, bool const encryptPacket = true)
    {
        // Save the value before it gets blowfished
        auto const type = p.type().has_value() ? fmt::format("0x{:02x}", p.type().value())
                                               : "<unknown>";

        p.writeChecksumAndSize();

        if (encryptPacket)
            blowfish.encrypt(p.body());

        socket.send(boost::asio::buffer(p.buffer(), p.size()));
        SPDLOG_INFO("sent: {} ({} bytes)", type, p.size());
    }
};

static void sendInitPacket(Connection & conn)
{
    static u32 sessionId = 0;

    constexpr u32 protocol = 0xc621;
    std::array<byte, 128> modulus;

    BIGNUM const * n = nullptr;
    RSA_get0_key(conn.rsaKey.get(), &n, nullptr, nullptr);
    BN_bn2bin(n, &modulus[0]);

    // scramble modulus
    {
        for (size_t i = 0; i < 4; ++i)
            std::swap(modulus[i], modulus[0x4d + i]);

        // step 2 xor first 0x40 bytes with last 0x40 bytes
        for (size_t i = 0; i < 0x40; ++i)
            modulus[i] = static_cast<byte>(modulus[i] ^ modulus[0x40 + i]);

        // step 3 xor bytes 0x0d-0x10 with bytes 0x34-0x38
        for (size_t i = 0; i < 4; ++i)
            modulus[0x0d + i] = static_cast<byte>(modulus[0x0d + i] ^ modulus[0x34 + i]);

        // step 4 xor last 0x40 bytes with first 0x40 bytes
        for (size_t i = 0; i < 0x40; ++i)
            modulus[0x40 + i] = static_cast<byte>(modulus[0x40 + i] ^ modulus[i]);
    }

    Packet p(SentPacket::Initialization);
    p << sessionId++ << protocol << modulus;
    conn.send(p, false);
}

static void handleGameGuardPacket(Connection & conn)
{
    constexpr u32 ignoreGg = 0x0b;
    conn.send(Packet(SentPacket::GameGuard) << ignoreGg);
}

static void handleAuthPacket(Connection & conn)
{
    auto const content = conn.readBuffer.data() + sizeof(u16) + sizeof(byte);

    // RSA in-place decrypt the content of the packet
    auto const decryptedSize = RSA_private_decrypt(RSA_size(conn.rsaKey.get()), content,
                                                   content, conn.rsaKey.get(), RSA_NO_PADDING);
    if (decryptedSize == -1)
    {
        auto const code = ERR_get_error();
        SPDLOG_ERROR("RSA_private_decrypt failed with code {}: {}",
                     code, ERR_error_string(code, nullptr));

        return conn.send(Packet(SentPacket::AuthenticationFailed) << 0x01); // system error
    }

    conn.userName = reinterpret_cast<char const *>(content + 0x62);
    conn.password = reinterpret_cast<char const *>(content + 0x70);

    if (conn.userName.size() > 14)
        conn.userName.resize(14);

    if (conn.password.size() > 16)
        conn.password.resize(16);

    SPDLOG_INFO("username: '{}' | password: '{}'", conn.userName, conn.password);

    u32 login1, login2;
    RAND_bytes(reinterpret_cast<byte *>(&login1), sizeof(login1));
    RAND_bytes(reinterpret_cast<byte *>(&login2), sizeof(login2));

    Packet p(SentPacket::AuthenticationSuccess);
    p << login1 << login2;
    conn.send(p);
}

static void handleServerListPacket(Connection & conn)
{
    struct ServerInfo
    {
        u8   id             = 1;
        u8   host[4]        = {127, 0, 0, 1};
        u32  port           = 7777;
        u8   ageLimit       = 0;
        bool isPvp          = false;
        u16  curPlayerCount = 0;
        u16  maxPlayerCount = 1'000;
        u8   status         = 1;
        u32  extra          = 0;
        u8   brackets       = 0;
    } const defaultServer;

    constexpr u8 serverCount = 1;

    Packet p(SentPacket::GameServerList);
    p << serverCount
      << u8(0) // unused or reserved
      << defaultServer.id
      << defaultServer.host
      << defaultServer.port
      << defaultServer.ageLimit
      << defaultServer.isPvp
      << defaultServer.curPlayerCount
      << defaultServer.maxPlayerCount
      << defaultServer.status
      << defaultServer.extra
      << defaultServer.brackets;

    conn.send(p);
}

static void handleServerSelectionPacket(Connection & conn)
{
    u32 login1, login2;
    RAND_bytes(reinterpret_cast<byte *>(&login1), sizeof(login1));
    RAND_bytes(reinterpret_cast<byte *>(&login2), sizeof(login2));

    Packet p(SentPacket::GameServerSelectionSuccess);
    p << login1 << login2;
    conn.send(p);
}

static PacketHandler readPacket(Connection & conn)
{
    conn.readBuffer.assign(conn.readBuffer.size(), '\0'); // Reset buffer

    u16 size;
    boost::asio::read(conn.socket, boost::asio::buffer(&size, sizeof(size)));
    std::memcpy(conn.readBuffer.data(), &size, sizeof(size));

    SPDLOG_TRACE("Incoming packet of size '{}'", size);

    if (conn.readBuffer.size() < size)
    {
        auto const oldSize = conn.readBuffer.size();
        conn.readBuffer.resize(size);
        SPDLOG_TRACE("readBuffer resized from '{}' to '{}'", oldSize, conn.readBuffer.size());
    }

    auto request  = conn.readBuffer.data() + sizeof(size);
    auto bodySize = size - sizeof(size);

    SPDLOG_TRACE("Attempt to read next {} bytes", bodySize);
    boost::asio::read(conn.socket, boost::asio::buffer(request, bodySize));

    // Blowfish decrypt
    conn.blowfish.decrypt({request, bodySize});

    // Read packet type
    auto const type = request[0];
    request  += sizeof(type);
    bodySize -= sizeof(type);

    SPDLOG_INFO("recv: 0x{:02X} ({} bytes)", type, size);

    void (*handle)(Connection & conn) = {};

    std::string text;
    switch (type)
    {
#define CASE(id) case static_cast<decltype(type)>(RecvPacket::id)
        CASE(Authentication):
        {
            text = "handle_auth_request";
            handle = &handleAuthPacket;
            break;
        }
        CASE(GameServerSelection):
        {
            text = "handle_game_server";
            handle = &handleServerSelectionPacket;
            break;
        }
        CASE(GameServerList):
        {
            text = "handle_server_list_request";
            handle = &handleServerListPacket;
            break;
        }
        CASE(GameGuard):
        {
            text = "ignore_gg_packet";
            handle = &handleGameGuardPacket;
            break;
        }
        default:
        {
            text = fmt::format("Unknown packet 0x{:02x}", type);
            break;
        }
#undef CASE
    }
    SPDLOG_INFO("Packet string type: {}", text);
    return handle;
}

int main() try
{
#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#)");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#:%!)");
#endif
    spdlog::set_level(spdlog::level::trace);

    boost::asio::io_context io;
    l2cpp::Network::SocketListener const listener(io);
    auto onSocketAccepted = [] (tcp::socket && socket)
    {
        SPDLOG_INFO("Client connected!");
        Connection conn(std::move(socket));
        sendInitPacket(conn);

        PacketHandler handle;
        while ((handle = readPacket(conn)))
            (*handle)(conn);
    };
    if (!listener.listen("127.0.0.1", 2106, std::move(onSocketAccepted)))
        return EXIT_FAILURE;

    io.run();

    return EXIT_SUCCESS;
}
catch (std::exception const & e)
{
    SPDLOG_CRITICAL("Exception caught: '{}'", e.what());
    return EXIT_FAILURE;
}
catch (...)
{
    SPDLOG_CRITICAL("Unknown exception caught");
    return EXIT_FAILURE;
}
