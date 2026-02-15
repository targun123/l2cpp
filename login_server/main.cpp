#include <boost/asio.hpp>
#include <iostream>
#include <openssl/blowfish.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <spdlog/spdlog.h>
#include <cstdlib>

using tcp = boost::asio::ip::tcp;

using byte = std::uint8_t;
using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;

struct Connection;
using PacketHandler = void (*)(Connection &);

template<typename T, typename D = std::default_delete<T>>
auto makeUPtr(T * t, D deleter = {}) -> decltype(auto) { return std::unique_ptr<T, D>(t, deleter); }

static void applyBlowfish(byte const * start, size_t const size,
                          BF_KEY const & blowfish, bool const mode)
{
    for (size_t i = 0; i < size; i += 8)
    {
        union { u32 ints[2]; byte raw[8]; } chunk = {0};
        chunk.ints[0] = htonl(*(u32 *) (start + i + 0));
        chunk.ints[1] = htonl(*(u32 *) (start + i + 4));
        BF_ecb_encrypt(chunk.raw, chunk.raw, &blowfish, mode);
        chunk.ints[0] = ntohl(chunk.ints[0]);
        chunk.ints[1] = ntohl(chunk.ints[1]);
        *(u32 *) (start + i + 0) = chunk.ints[0];
        *(u32 *) (start + i + 4) = chunk.ints[1];
    }
}

class Packet
{
public:
    Packet()
    {
        _buffer.reserve(256);
        append<u16>(0); // slot to write final size before sending
    }

    explicit Packet(byte const type)
        : Packet()
    {
        _buffer.emplace_back(type);
    }

    Packet & append(byte const b)
    {
        _buffer.emplace_back(b);
        return *this;
    }

    template<typename T>
    Packet & append(T const & t)
    {
        std::array<byte, sizeof(T)> chunk;
        std::memcpy(&chunk[0], &t, sizeof(T));
        _buffer.append_range(chunk);
        return *this;
    }

    template<typename T>
    Packet & operator<<(T const & t) { return append(t); }

    void finalize()
    {
        u32 result = 0;

        auto const buf  = _buffer.data();
        for (auto start = buf + sizeof(u16), end = buf + size(); start + 4 < end; )
        {
            u32 ecx = *start++ & 0xff;
            ecx |= (*start++ << 8) & 0xff00;
            ecx |= (*start++ << 0x10) & 0xff0000;
            ecx |= (*start++ << 0x18) & 0xff000000;
            result ^= ecx;
        }

        append(result);

        while (bodySize() % 8 != 0) // Pad to 8 bytes with zeroes
            _buffer.emplace_back(0);

        // Write total size on the first two bytes of the buffer
        auto const finalSize = static_cast<u16>(size());
        std::memcpy(_buffer.data(), &finalSize, sizeof(finalSize));
    }

public:
    auto buffer()    const -> std::vector<byte> const & { return _buffer;        }
    auto rawBuffer() const -> byte const *              { return _buffer.data(); }

    auto size() const -> size_t { return _buffer.size(); }
    auto type() const -> byte   { return _buffer.size() > sizeof(u16) ? _buffer[2] : 0xFF; }

    auto bodySize() const -> size_t       { return _buffer.size() - sizeof(u16); }
    auto body()     const -> byte const * { return _buffer.data() + sizeof(u16); }

private:
    std::vector<byte> _buffer;
};

struct Connection
{
    tcp::socket socket;
    BF_KEY      blowfish;

    std::unique_ptr<RSA,    decltype(&RSA_free)> rsaKey;
    std::unique_ptr<BIGNUM, decltype(&BN_free)>  bigNum;

    std::vector<byte> readBuffer;

    std::string username;
    std::string password;

    explicit Connection(tcp::socket socket)
        : socket(std::move(socket))
        , rsaKey(RSA_new(), &RSA_free)
        , bigNum(nullptr, &BN_free)
    {
        constexpr unsigned char key[] = "_;5.]94-31==-%xT!^[$";
        BF_set_key(&blowfish, sizeof(key), key);

        BIGNUM * n = nullptr;
        BN_dec2bn(&n, "65537");
        bigNum.reset(n);

        RSA_generate_key_ex(rsaKey.get(), 1024, bigNum.get(), nullptr);
        readBuffer.resize(sizeof(u16) + sizeof(u8) + RSA_size(rsaKey.get())); // size + type + rsa
    }

    void send(Packet & p, bool const encryptPacket = true)
    {
        auto const type = p.type();

        p.finalize();

        if (encryptPacket)
            applyBlowfish(p.body(), p.bodySize(), blowfish, BF_ENCRYPT);

        socket.send(boost::asio::buffer(p.buffer(), p.size()));
        SPDLOG_INFO("sent: 0x{:02x} ({} bytes)", type, p.size());
    }
};

static void hexdump(void const * ptr, size_t const buflen)
{
    std::string result;
    result.reserve(buflen / 16 + (buflen % 16 ? 16 : 0));

    auto const buf = static_cast<unsigned char const *>(ptr);
    for (int i = 0; i < buflen; i += 16)
    {
        result += fmt::format("{:06X}: ", i);
        for (int j = 0; j < 16; ++j)
        {
            if (i + j < buflen)
                result += fmt::format("{:02X} ", buf[i + j]);
            else
                result += fmt::format(".. ");
        }
        for (int j = 0; j < 16; ++j)
        {
            if (i + j < buflen)
                result += fmt::format("{:c}", std::isprint(buf[i + j]) ? buf[i + j] : '.');
            else
                result += fmt::format(".");
        }
        result += fmt::format("\n");
    }

    fmt::print("{}", result);
    fflush(stdout);
}

static void sendInitPacket(Connection & conn)
{
    struct {
        byte session_id[4];
        byte protocol[4];
        byte modulus[128];
    } init = {
        {0xfd, 0x8a, 0x22, 0x00}, // SessionID (why this value?)
        {0x5a, 0x78, 0x00, 0x00}, // C4 protocol 785a vs. c621
        {0},
    };

    BIGNUM const * n = nullptr;
    RSA_get0_key(conn.rsaKey.get(), &n, nullptr, nullptr);
    BN_bn2bin(n, init.modulus);

    // scramble modulus
    // credits: l2j
    {
        auto const modulus = init.modulus;
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

    Packet p(0x00);
    p << init.session_id << init.protocol << init.modulus;
    conn.send(p, false);
}

static void handleGameGuardPacket(Connection & conn)
{
    constexpr u32 ignoreGg = 0x0b;
    conn.send(Packet(0x0b) << ignoreGg);
}

static void handleAuthPacket(Connection & conn)
{
    auto const body = conn.readBuffer.data() + sizeof(u16) + sizeof(byte);
    conn.username   = reinterpret_cast<char const *>(body + 0x62);
    conn.password   = reinterpret_cast<char const *>(body + 0x70);

    SPDLOG_INFO("username: '{}' | password: '{}'", conn.username, conn.password);

    u32 login1, login2;
    RAND_bytes(reinterpret_cast<byte *>(&login1), sizeof(login1));
    RAND_bytes(reinterpret_cast<byte *>(&login2), sizeof(login2));

    constexpr byte unknown[] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xea, 0x03, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x60, 0x62, 0xe0, 0x00,
        0x00, 0x00, 0x00,
    };

    Packet p(0x03);
    p << login1 << login2 << unknown;
    conn.send(p);
}

static void handleServerListPacket(Connection & conn)
{
    struct ServerInfo
    {
        u8   id             = 1;
        u32  host           = 0;
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

    Packet p(0x04);
    p << serverCount
      << u8(0)
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
    applyBlowfish(request, bodySize, conn.blowfish, BF_DECRYPT);

    // Read packet type
    auto const type = request[0];
    request  += sizeof(type);
    bodySize -= sizeof(type);

    SPDLOG_INFO("recv: 0x{:02X} ({} bytes)", type, size);

    if (type == 0x00)
    {
        // RSA in-place decrypt the body of the packet
        auto const decryptedSize = RSA_private_decrypt(RSA_size(conn.rsaKey.get()), request,
                                                       request, conn.rsaKey.get(), RSA_NO_PADDING);
        if (decryptedSize == -1)
        {
            auto const code = ERR_get_error();
            SPDLOG_ERROR("RSA_private_decrypt failed with code {}: {}",
                         code, ERR_error_string(code, nullptr));
        }
    }

    void (*handle)(Connection & conn) = {};

    std::string_view text;
    switch (type)
    {
        case 0x00:
        {
            text = "handle_auth_request";
            handle = &handleAuthPacket;
            break;
        }
        case 0x02:
        {
            text = "handle_game_server";
            break;
        }
        case 0x05:
        {
            text = "handle_server_list_request";
            handle = &handleServerListPacket;
            break;
        }
        case 0x07:
        {
            text = "ignore_gg_packet";
            handle = &handleGameGuardPacket;
            break;
        }
        default:
        {
            text = "unknown packet";
            break;
        }
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
    tcp::acceptor acceptor{io};
    tcp::endpoint const endpoint { tcp::v4(), 2106 };
    acceptor.open(endpoint.protocol());
    acceptor.set_option(tcp::acceptor::reuse_address{true});
    acceptor.bind(endpoint);
    acceptor.listen(boost::asio::socket_base::max_listen_connections);
    acceptor.async_accept([] (boost::system::error_code const & ec, tcp::socket socket)
    {
        if (ec)
        {
            SPDLOG_ERROR("Socket error {}: {}", ec.default_error_condition().value(),
                                                ec.default_error_condition().message());
            socket.close();
        }
        else
        {
            SPDLOG_INFO("Socket connected!");
            Connection conn(std::move(socket));
            sendInitPacket(conn);

            PacketHandler handle;
            while ((handle = readPacket(conn)))
                (*handle)(conn);

            socket.close();
        }
    });
    boost::asio::post(io, [&]
    {
        auto const addr = endpoint.address().to_string();
        SPDLOG_INFO("Listening on {}:{}", (addr == "0.0.0.0" ? "*" : addr), endpoint.port());
    });

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
