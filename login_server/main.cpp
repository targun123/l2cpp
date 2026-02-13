#include <boost/asio.hpp>
#include <iostream>
#include <openssl/blowfish.h>
#include <openssl/rsa.h>
#include <spdlog/spdlog.h>
#include <cstdlib>

using tcp = boost::asio::ip::tcp;

using byte = unsigned char;
using u16  = std::uint16_t;
using u32  = std::uint32_t;

#define memAppend(dest, src) \
    (memcpy((dest), &(src), sizeof(src)), (dest) + sizeof(src))

template<typename D, typename S>
void * appendF(D * dest, S const * src)
{
    std::memcpy(dest, src, sizeof(src));
    return dest + sizeof(src);
}

struct Connection
{
    tcp::socket socket;
    BF_KEY      blowfish;

    std::unique_ptr<RSA,    decltype(&RSA_free)> rsaKey;
    std::unique_ptr<BIGNUM, decltype(&BN_free)>  bigNum;

    Connection(tcp::socket socket) noexcept
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
    }
};

struct Packet
{
    std::vector<byte> buffer;
    Packet()
    {
        buffer.reserve(1024);
        append<u16>(0);
    }

    template<typename T>
    Packet & append(T const & t)
    {
        byte chunk[sizeof(T)];
        memcpy(chunk, &t, sizeof(T));
        buffer.append_range(chunk);
        return *this;
    }

    template<typename T>
    Packet & operator<<(T const & t) { return append(t); }
};

template<typename T, typename D>
auto makeUPtr(T * t, D deleter) -> decltype(auto) { return std::unique_ptr<T, D>(t, deleter); }

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
}

static u16 checksum(byte *dest, byte *start, byte *end)
{
    assert(dest);
    assert(start);
    assert(end);
    assert(start < end);
    assert(end - start < std::numeric_limits<u16>::max());

    u16 size = static_cast<u16>(end - start);
    u32 result = 0;
    for (u16 i = 0; i < size; i += 4)
    {
        u32 ecx = *start++ & 0xff;
        ecx |= (*start++ << 8) & 0xff00;
        ecx |= (*start++ << 0x10) & 0xff0000;
        ecx |= (*start++ << 0x18) & 0xff000000;
        result ^= ecx;
    }

    memAppend(end, result);
    size += static_cast<u16>(sizeof(result));

    /*
     * The packet must be multiple of 8
     */
    u16 const body_padded_size = ((size + 7) & (~7));
    constexpr u16 size_header = 2;
    /*
     * The final size of the packet consists of
     * the padded size plus 2 bytes used to store how
     * big the packet is.
     */
    u16 const final_size = body_padded_size + size_header;
    memcpy(dest, &final_size, sizeof(final_size));

    return final_size;
}

static void readPacket(Connection & conn)
{
    std::vector<byte> readBuffer(1024);
    auto const netSize = conn.socket.receive(boost::asio::buffer(readBuffer));

    auto request = readBuffer.data();

    // Read packet size
    u16 size = 0;
    memcpy(&size, request, sizeof(size));
    request += sizeof(size);

    auto const bodySize = size - sizeof(size);

    // Blowfish decrypt
    for (u16 i = 0; i < bodySize; i += 8)
    {
        union { u32 ints[2]; byte raw[8]; } chunk = {0};
        chunk.ints[0] = htonl(*(u32 *) (request + i + 0));
        chunk.ints[1] = htonl(*(u32 *) (request + i + 4));

        BF_ecb_encrypt(chunk.raw, chunk.raw, &conn.blowfish, BF_DECRYPT);
        chunk.ints[0] = ntohl(chunk.ints[0]);
        chunk.ints[1] = ntohl(chunk.ints[1]);
        *(u32 *) (request + i + 0) = chunk.ints[0];
        *(u32 *) (request + i + 4) = chunk.ints[1];
    }

    // Read packet type
    byte type = 0;
    memcpy(&type, request, sizeof(type));
    request += sizeof(type);

    SPDLOG_INFO("netSize = '{}' | packetSize = '{}' | bodySize = '{}' | type = 0x{:02X}",
                netSize, size, bodySize, type);

    // RSA decrypt the body of the packet
    RSA_private_decrypt(RSA_size(conn.rsaKey.get()),
                        request, request, conn.rsaKey.get(), RSA_NO_PADDING);

    std::string_view text;
    switch (type)
    {
        case 0x00: text = "handle_auth_request";        break;
        case 0x02: text = "handle_game_server";         break;
        case 0x05: text = "handle_server_list_request"; break;
        case 0x07: text = "ignore_gg_packet";           break;
        default:   text = "unknown packet";             break;
    }
    SPDLOG_INFO("Packet string type: {}", text);
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
        for (int i = 0; i < 4; i++)
            std::swap(modulus[i], modulus[0x4d + i]);

        // step 2 xor first 0x40 bytes with last 0x40 bytes
        for (int i = 0; i < 0x40; i++)
            modulus[i] = static_cast<byte>(modulus[i] ^ modulus[0x40 + i]);

        // step 3 xor bytes 0x0d-0x10 with bytes 0x34-0x38
        for (int i = 0; i < 4; i++)
            modulus[0x0d + i] = static_cast<byte>(modulus[0x0d + i] ^ modulus[0x34 + i]);

        // step 4 xor last 0x40 bytes with first 0x40 bytes
        for (int i = 0; i < 0x40; i++)
            modulus[0x40 + i] = static_cast<byte>(modulus[0x40 + i] ^ modulus[i]);
    }

    Packet p;
    p << u16(0) << init.session_id << init.protocol << init.modulus;

    auto const buf    = p.buffer.data();
    size_t const size = checksum(buf, buf + sizeof(u16), buf + p.buffer.size());
    SPDLOG_INFO("Sending packet of size '{}' (net size = '{}')", size, p.buffer.size());
    conn.socket.send(boost::asio::buffer(buf, size));
}

int main() try
{
#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#)");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#:%!)");
#endif

    boost::asio::io_context io;
    boost::asio::post(io, [] { SPDLOG_INFO("Hello, World!"); });

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
            readPacket(conn);
            socket.close();
        }
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
