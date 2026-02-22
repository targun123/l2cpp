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
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

// C++ includes
#include <iostream>
#include <numeric>
#include <span>

using boost::asio::ip::tcp;

static void encrypt(std::span<byte> data, std::array<byte, sizeof(u64)> & key)
{
    L2CPP_B_ASSERT(data.size() <= std::numeric_limits<u16>::max(), "Data size ({}) > UINT16_MAX", data.size());

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i] & 0xFF;
        data[i] = static_cast<byte>(tmp2 ^ (key[i & 7] & 0xFF) ^ tmp1);
        tmp1    = data[i];
    }

    tmp1  = (key[0] << 0x00) & 0xFF;
    tmp1 |= (key[1] << 0x08) & 0xFF00;
    tmp1 |= (key[2] << 0x10) & 0xFF0000;
    tmp1 |= (key[3] << 0x18) & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    key[0] = static_cast<byte>((tmp1 >> 0x00) & 0xFF);
    key[1] = static_cast<byte>((tmp1 >> 0x08) & 0xFF);
    key[2] = static_cast<byte>((tmp1 >> 0x10) & 0xFF);
    key[3] = static_cast<byte>((tmp1 >> 0x18) & 0xFF);
}

static void decrypt(std::span<byte> data, std::array<byte, sizeof(u64)> & key)
{
    L2CPP_B_ASSERT(data.size() <= std::numeric_limits<u16>::max(), "Data size ({}) > UINT16_MAX", data.size());

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i] & 0xFF;
        data[i] = static_cast<byte>(tmp2 ^ (key[i & 7] & 0xFF) ^ tmp1);
        tmp1    = tmp2;
    }

    tmp1  = (key[0] << 0x00) & 0xFF;
    tmp1 |= (key[1] << 0x08) & 0xFF00;
    tmp1 |= (key[2] << 0x10) & 0xFF0000;
    tmp1 |= (key[3] << 0x18) & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    key[0] = static_cast<byte>((tmp1 >> 0x00) & 0xFF);
    key[1] = static_cast<byte>((tmp1 >> 0x08) & 0xFF);
    key[2] = static_cast<byte>((tmp1 >> 0x10) & 0xFF);
    key[3] = static_cast<byte>((tmp1 >> 0x18) & 0xFF);
}

struct Connection
{
    tcp::socket       socket;
    std::vector<byte> readBuffer;
    std::array<byte, sizeof(u64)> encryptionKey, decryptionKey;

    explicit Connection(tcp::socket socket)
        : socket(std::move(socket))
        , encryptionKey{{0x94, 0x35, 0x00, 0x00, 0xa1, 0x6c, 0x54, 0x87}}
        , decryptionKey(encryptionKey)
    {
        readBuffer.resize(sizeof(u16));
    }

    void send(Packet & p, bool const needEncryption = true)
    {
        p.writeSize();

        SPDLOG_DEBUG("About to send packet 0x{:02x} ({} bytes)", p.opCode().value_or(0xFF), p.size());
        std::cout << l2cpp::hexdump(p.buffer().data(), p.buffer().size()) << std::endl;

        if (needEncryption)
            encrypt(p.body(), encryptionKey);

        socket.send(boost::asio::buffer(p.buffer()));
    }
};

static void readPacket(Connection & conn, bool const needDecryption = true)
{
    // Reset buffer
    std::ranges::fill(conn.readBuffer, 0);

    // Read packet size
    boost::system::error_code ec;
    boost::asio::read(conn.socket, boost::asio::buffer(conn.readBuffer.data(), sizeof(u16)), ec);
    if (ec.value() == boost::asio::error::eof)
        L2CPP_THROW("Client disconnected");

    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    auto const size = *reinterpret_cast<u16 *>(conn.readBuffer.data());
    L2CPP_B_ASSERT(size, "Packet without body");
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
    boost::asio::read(conn.socket, boost::asio::buffer(request, bodySize), ec);
    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    if (needDecryption)
        decrypt({request, bodySize}, conn.decryptionKey);

    // Read packet type
    auto const type = request[0];
    SPDLOG_INFO("recv: 0x{:02X} ({} bytes)", type, size);
    std::cout << l2cpp::hexdump(conn.readBuffer.data(), size) << std::endl;
}

static void handleProtocol(Connection & conn)
{
    auto const protocol = *reinterpret_cast<u32 *>(conn.readBuffer.data() + sizeof(u16) + sizeof(u8));
    SPDLOG_INFO("Client protocol: {}", protocol);

    conn.send(Packet(0x00).append<u8>(1) << conn.encryptionKey, false);
}

static void handleAuth(Connection & conn)
{
    auto content = conn.readBuffer.data() + sizeof(u16) + sizeof(u8);

    std::wstring userName = reinterpret_cast<wchar_t const *>(content);
    content += userName.size() * sizeof(wchar_t) + sizeof(wchar_t);

    u32 playOk1, playOk2, loginOk1, loginOk2;
    for (auto const n : {&playOk1, &playOk2, &loginOk1, &loginOk2})
    {
        *n = *reinterpret_cast<u32 *>(content);
        content += sizeof(u32);
    }

    SPDLOG_DEBUG(L"'{}' | {} | {} | {} | {}", userName, loginOk1, loginOk2, playOk1, playOk2);
}

void handleCharacterList(Connection & conn)
{
    u32 count = 1;
    u32 id = 0;
    wchar_t name[] = L"MyNameIsAdmin";
    u32 clanId = 0;
    u32 sex = 0;
    u32 raceId = 0;
    u32 active = 1;
    double currentHp = 500;
    double currentMp = 500;
    u32 sp = 0;
    u32 xp = 0;
    u32 level = 1;
    u32 karma = 0;
    u32 hairStyleId = 0;
    u32 hairColorId = 0;
    u32 faceId = 0;
    double maxHp = 500;
    double maxMp = 500;
    u32 deleteTime = 0;
    u32 classId = 0;
    u32 autoSelect = 0;
    u8 enchantEffect = 0;

    Packet p(0x13);
    p << 0;
    conn.send(p);
    // p << count
    //   << name
    //   << id
    //   << L"AccountName"
    //   << 0
    //   << clanId
    //   << 0
    //   << sex
    //   << raceId
    //   << classId
    //   << active
    //   << 0 // x
    //   << 0 // y
    //   << 0 // z
    //   << currentHp
    //   << currentMp
    //   << sp
    //   << xp
    //   << level
    //   << karma;
    //
    // // ???
    // p << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0
    //   << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0
    //   << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0
    //   << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
    //
    // p << hairStyleId
    //   << hairColorId
    //   << faceId
    //   << maxHp
    //   << maxMp
    //   << deleteTime
    //   << classId
    //   << autoSelect
    //   << enchantEffect;
    //
    // conn.send(p);
}

void handleCharacterCreationScreen(Connection & conn)
{
    conn.send(Packet(0x17) << 0);
}

void handleCharacterCreation(Connection & conn)
{
    conn.send(Packet(0x19) << 1);
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

    boost::asio::io_context        ioContext;
    l2cpp::Network::SocketListener socketListener(ioContext);

    auto onSocketAccepted = [] (tcp::socket socket)
    {
        Connection conn(std::move(socket));

        try
        {
            readPacket(conn, false); // First packet is not encrypted
            handleProtocol(conn);

            readPacket(conn);
            handleAuth(conn);
            handleCharacterList(conn);

            using PacketHandler = void(*)(Connection & conn);
            std::unordered_map<byte, PacketHandler> const handlers
            {
                { 0x0e, &handleCharacterCreationScreen },
                { 0x0b, &handleCharacterCreation       },
            };

            while (true)
            {
                readPacket(conn);
                if (auto const it = handlers.find(conn.readBuffer[2]); it != handlers.end())
                    (*it->second)(conn);
                else
                    SPDLOG_INFO("Unsupported packet 0x{:02x}", conn.readBuffer[2]);
            }
        }
        catch (l2cpp::Exception const & e)
        {
            SPDLOG_ERROR("Packet reading failed, disconnecting client:\n{}", l2cpp::formatExceptionStack(e));
        }
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
