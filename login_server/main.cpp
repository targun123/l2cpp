/// @author    Chnossos
/// @date      Created on 2026-02-13

// Project includes
#include "Blowfish.hpp"
#include "Packets.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party includes
#include <boost/asio.hpp>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <spdlog/spdlog.h>

using boost::asio::ip::tcp;
using l2cpp::Network::Packet;

struct Connection;
using PacketHandler = void (*)(Connection &);

constexpr byte gBlowfishToken[] = "_;5.]94-31==-%xT!^[$"; // trailing zero is included by sizeof()

namespace
{
    u64 calculateChecksum(std::span<byte const> const data)
    {
        L2CPP_B_ASSERT(data.size() >= sizeof(u32), "Cannot calculate checksum: size < sizeof(u32)");

        u32 checksum = 0;

        for (size_t i = 0; i < data.size(); i += sizeof(u32))
            checksum ^= *reinterpret_cast<u32 const *>(data.data() + i);

        return checksum; // pad to 8 bytes, required by Blowfish
    }

    bool verifyChecksum(std::span<byte const> const data)
    {
        auto       start = reinterpret_cast<u32 const *>(data.data());
        auto const end   = reinterpret_cast<u32 const *>(data.data() + data.size() - sizeof(u64));

        u32 checksum = 0;
        while (start < end)
            checksum ^= *start++;

        return checksum == *start;
    }
}

struct Connection
{
    u32             sessionId;
    tcp::socket     socket;
    l2cpp::Blowfish blowfish;

    std::unique_ptr<BIGNUM, decltype(&BN_free)>  bigNum;
    std::unique_ptr<RSA,    decltype(&RSA_free)> rsaKey;

    std::vector<byte> readBuffer;

    std::string userName;
    std::string password;

    std::function<void()>                          onSocketClosed;
    std::function<void(std::vector<byte> const &)> onPacketRead;

    explicit Connection(tcp::socket socket)
        : socket(std::move(socket))
        , blowfish(gBlowfishToken)
        , bigNum(nullptr, &BN_free)
        , rsaKey(RSA_new(), &RSA_free)
    {
        static decltype(sessionId) nextSessionId = 0;
        sessionId = nextSessionId++;

        BIGNUM * n = nullptr;
        BN_dec2bn(&n, "65537");
        bigNum.reset(n);

        RSA_generate_key_ex(rsaKey.get(), 1024, bigNum.get(), nullptr);
        readBuffer.resize(sizeof(u16) + sizeof(u8) + RSA_size(rsaKey.get())); // size + opCode + rsa
    }

    bool operator==(Connection const & other) const { return sessionId == other.sessionId; }

    void close()
    {
        if (socket.is_open())
        {
            boost::system::error_code ec;
            if (socket.shutdown(socket.shutdown_both, ec))
                SPDLOG_ERROR("shutdown error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

            if (socket.close(ec))
                SPDLOG_ERROR("close error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

            if (onSocketClosed)
                onSocketClosed();
        }
    }

    void asyncReadNextPacket()
    {
        L2CPP_B_ASSERT(socket.is_open(), "Cannot read another packet: socket not open");
        boost::asio::async_read(socket, boost::asio::buffer(readBuffer, sizeof(PacketHeader)),
                                [this] (auto const & ec, auto) { onSizeRead(ec); });
    }

    void send(Packet & p, bool const encryptPacket = true)
    {
        // Align to 8 bytes then append checksum
        while (p.bodySize() % 8 != 0)
            p << 0_u8;

        p << calculateChecksum({p.body().data(), p.bodySize()});
        p.finalize();

        if (encryptPacket)
            blowfish.encrypt(p.body());

        boost::asio::write(socket, boost::asio::buffer(p.buffer()));
        SPDLOG_INFO("'{}' ← 0x{:02x} ({} bytes)", sessionId, p.opCode(), p.size());
    }

private:
    void onSizeRead(boost::system::error_code const & ec)
    {
        if (!handleError(ec))
            return close();

        readBuffer.resize(*reinterpret_cast<PacketHeader const *>(readBuffer.data()));

        auto const payload = std::span(readBuffer).subspan(sizeof(PacketHeader));
        boost::asio::async_read(socket, boost::asio::buffer(payload), [this] (auto const & e, auto) { onBodyRead(e); });
    }

    void onBodyRead(boost::system::error_code const & ec)
    {
        if (!handleError(ec))
            return close();

        if (onPacketRead)
            onPacketRead(readBuffer);
    }

    /// @returns @c true if the async operation can be executed again, else @c false
    bool handleError(boost::system::error_code const & ec) const
    {
        switch (auto const code = ec.value())
        {
            case boost::system::errc::success:
                return true;

            case boost::asio::error::eof:
                SPDLOG_INFO("Client {} disconnected by themselves", sessionId);
                break;

            case boost::asio::error::operation_aborted:
                SPDLOG_INFO("Client {} disconnected by server", sessionId);
                break;

            default:
                SPDLOG_ERROR("Something went wrong during packet body reading (code {}): {}", code, ec.message());
                break;
        }
        return false;
    }
};

static void sendInitPacket(Connection & conn)
{
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
    p << conn.sessionId << protocol << modulus;
    conn.send(p, false);

    conn.asyncReadNextPacket();
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

    u32 loginOk1, loginOk2;
    RAND_bytes(reinterpret_cast<byte *>(&loginOk1), sizeof(loginOk1));
    RAND_bytes(reinterpret_cast<byte *>(&loginOk2), sizeof(loginOk2));

    Packet p(SentPacket::AuthenticationSuccess);
    p << loginOk1 << loginOk2;
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
        bool isOnline       = true;
        u32  extra          = 0;
        u8   brackets       = 0;
    } const onlineServer, offlineServer{.id = 2, .isPvp = true, .isOnline = false};

    constexpr u8 serverCount = 2;

    Packet p(SentPacket::GameServerList);
    p << serverCount << 0_u8; // ?

    for (auto const srv : {&onlineServer, &offlineServer})
    {
        p
            << srv->id
            << srv->host
            << srv->port
            << srv->ageLimit
            << srv->isPvp
            << srv->curPlayerCount
            << srv->maxPlayerCount
            << srv->isOnline
            << srv->extra
            << srv->brackets
        ;
    }

    conn.send(p);
}

static void handleServerSelectionPacket(Connection & conn)
{
    u32 playOk1, playOk2;
    RAND_bytes(reinterpret_cast<byte *>(&playOk1), sizeof(playOk1));
    RAND_bytes(reinterpret_cast<byte *>(&playOk2), sizeof(playOk2));

    Packet p(SentPacket::GameServerSelectionSuccess);
    p << playOk1 << playOk2;
    conn.send(p);
}

static void handlePacket(Connection & conn)
{
    auto const payload = std::span(conn.readBuffer).subspan(sizeof(PacketHeader));
    conn.blowfish.decrypt(payload);
    L2CPP_B_ASSERT(verifyChecksum(payload), "Checksum verification failed");

    auto const opCode = payload[0];
    SPDLOG_INFO("'{}' → 0x{:02x} ({} bytes)", conn.sessionId, opCode, conn.readBuffer.size());

    static std::unordered_map<RecvPacket, void(*)(Connection &)> const handlers
    {
        { RecvPacket::Authentication,      &handleAuthPacket            },
        { RecvPacket::GameServerSelection, &handleServerSelectionPacket },
        { RecvPacket::GameServerList,      &handleServerListPacket      },
        { RecvPacket::GameGuard,           &handleGameGuardPacket       },
    };

    if (auto const it = handlers.find(static_cast<RecvPacket>(opCode)); it != handlers.end()) try
    {
        (*it->second)(conn);
    }
    catch (l2cpp::Exception const & e)
    {
        SPDLOG_ERROR("Something wrong happened during packet handling of connection {}. Closing connection."
                     "\nBacktrace:\n{}", conn.sessionId, l2cpp::formatExceptionStack(e));
        return conn.close();
    }
    else
        SPDLOG_WARN("Received unknown packet type '{}'", opCode);

    conn.asyncReadNextPacket();
}

int main() try
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#]");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#] [%!()]");
#endif
    spdlog::set_level(spdlog::level::trace);

    std::list<Connection> connections;

    boost::asio::io_context io;
    l2cpp::Network::SocketListener listener(io);
    auto onSocketAccepted = [&] (tcp::socket && socket)
    {
        SPDLOG_INFO("Client connected!");
        auto & conn = connections.emplace_back(std::move(socket));
        conn.onSocketClosed = [&] { connections.remove(conn);          };
        conn.onPacketRead   = [&] (auto const &) { handlePacket(conn); };
        sendInitPacket(conn);
    };
    if (!listener.listen("127.0.0.1", 2106, std::move(onSocketAccepted)))
        return EXIT_FAILURE;

    SPDLOG_INFO("Listening for clients on 127.0.0.1:2106");
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
