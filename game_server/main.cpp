/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <numeric>
#include <span>

using boost::asio::ip::tcp;

static void encrypt(std::span<byte> data, std::array<byte, 16> & key)
{
    L2CPP_B_ASSERT(data.size() > std::numeric_limits<u16>::max(), "Data size > UINT16_MAX");

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i] & 0xFF;
        data[i] = static_cast<byte>(tmp2 ^ (key[i & 15] & 0xFF) ^ tmp1);
        tmp1    = data[i];
    }

    tmp1  = (key[ 8] << 0x00) & 0xFF;
    tmp1 |= (key[ 9] << 0x08) & 0xFF00;
    tmp1 |= (key[10] << 0x10) & 0xFF0000;
    tmp1 |= (key[11] << 0x18) & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    key[ 8] = static_cast<byte>((tmp1 >> 0x00) & 0xFF);
    key[ 9] = static_cast<byte>((tmp1 >> 0x08) & 0xFF);
    key[10] = static_cast<byte>((tmp1 >> 0x10) & 0xFF);
    key[11] = static_cast<byte>((tmp1 >> 0x18) & 0xFF);
}

static void decrypt(std::span<byte> data, std::array<byte, 16> & key)
{
    L2CPP_B_ASSERT(data.size() > std::numeric_limits<u16>::max(), "Data size > UINT16_MAX");

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i] & 0xFF;
        data[i] = static_cast<byte>(tmp2 ^ (key[i & 15] & 0xFF) ^ tmp1);
        tmp1    = tmp2;
    }

    tmp1  = (key[ 8] << 0x00) & 0xFF;
    tmp1 |= (key[ 9] << 0x08) & 0xFF00;
    tmp1 |= (key[10] << 0x10) & 0xFF0000;
    tmp1 |= (key[11] << 0x18) & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    key[ 8] = static_cast<byte>((tmp1 >> 0x00) & 0xFF);
    key[ 9] = static_cast<byte>((tmp1 >> 0x08) & 0xFF);
    key[10] = static_cast<byte>((tmp1 >> 0x10) & 0xFF);
    key[11] = static_cast<byte>((tmp1 >> 0x18) & 0xFF);
}

struct Connection
{
    tcp::socket          socket;
    std::vector<byte>    readBuffer;
    std::array<byte, 16> encryptionKey;
    std::array<byte, 16> decryptionKey;

    explicit Connection(tcp::socket socket)
        : socket(std::move(socket))
    {
        readBuffer.resize(sizeof(u16));

        std::iota(encryptionKey.begin(), encryptionKey.end(), 0); // TODO: rand
        std::iota(decryptionKey.begin(), decryptionKey.end(), 0); // TODO: rand
    }
};

static void readPacket(Connection & conn)
{
    // First packet is not encrypted, so check if buffer hasn't grown yet from reading any packet
    bool const needsDecrypt = conn.readBuffer.size() == sizeof(16);

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

    if (needsDecrypt) [[likely]]
        decrypt({request, bodySize}, conn.decryptionKey);

    // Blowfish decrypt
    // applyBlowfish({request, bodySize}, conn.blowfish, BF_DECRYPT);

    // Read packet type
    auto const type = request[0];
    request  += sizeof(type);
    bodySize -= sizeof(type);

    SPDLOG_INFO("recv: 0x{:02X} ({} bytes)", type, size);
    std::cout << l2cpp::hexdump(request, bodySize) << std::flush;
}

int main() try
{
#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#]");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#] [%!()]");
#endif
    spdlog::set_level(spdlog::level::trace);

    constexpr auto ip   = "127.0.0.1";
    constexpr auto port = 7777;

    boost::asio::io_context ioContext;
    l2cpp::Network::SocketListener socketListener(ioContext);

    auto onSocketAccepted = [] (tcp::socket socket)
    {
        Connection conn(std::move(socket));
        readPacket(conn); // Ignore protocol
    };
    L2CPP_B_ASSERT(socketListener.listen(ip, port, std::move(onSocketAccepted)),
                   "Failed to listen on {}:{}", ip, port);

    ioContext.run();
    return EXIT_SUCCESS;
}
catch (l2cpp::Exception const & e)
{
    l2cpp::printExceptionStack(e);
    return EXIT_FAILURE;
}
catch (std::exception const & e)
{
    l2cpp::printExceptionStack(e);
    return EXIT_FAILURE;
}
catch (...)
{
    SPDLOG_CRITICAL("Unexpected exception caught, terminating");
    return EXIT_FAILURE;
}
