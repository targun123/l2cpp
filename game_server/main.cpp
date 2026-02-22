/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include "network/Connection.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

// C++ includes
#include <iostream>

void onSocketAccepted(boost::asio::ip::tcp::socket && socket);

static void handleProtocol(Connection & conn)
{
    auto const protocol = *reinterpret_cast<u32 const *>(conn.readBuffer().data() + sizeof(u16) + sizeof(u8));
    SPDLOG_INFO("Client protocol: {}", protocol);

    conn.send(Packet(0x00).append<u8>(1) << conn.encryptionKey(), false);
}

static void handleAuth(Connection & conn)
{
    auto content = conn.readBuffer().data() + sizeof(u16) + sizeof(u8);

    std::wstring userName = reinterpret_cast<wchar_t const *>(content);
    content += userName.size() * sizeof(wchar_t) + sizeof(wchar_t);

    u32 playOk1, playOk2, loginOk1, loginOk2;
    for (auto const n : {&playOk1, &playOk2, &loginOk1, &loginOk2})
    {
        *n = *reinterpret_cast<u32 const *>(content);
        content += sizeof(u32);
    }

    SPDLOG_DEBUG(L"'{}' | {} | {} | {} | {}", userName, loginOk1, loginOk2, playOk1, playOk2);
}

static void handleCharacterList(Connection & conn)
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

static void handleConnectionClosing(Connection & conn)
{
    conn.close();
}

static void handleCharacterCreationScreen(Connection & conn)
{
    conn.send(Packet(0x17) << 0);
}

static void handleCharacterCreation(Connection & conn)
{
    conn.send(Packet(0x19) << 1);
}

static void onSocketAccepted(boost::asio::ip::tcp::socket && socket) try
{
    Connection conn(std::move(socket));
    conn.read(false); // First packet is not encrypted
    handleProtocol(conn);

    conn.read();
    handleAuth(conn);
    handleCharacterList(conn);

    using PacketHandler = void(*)(Connection & conn);
    std::unordered_map<byte, PacketHandler> const handlers
    {
        { 0x09, &handleConnectionClosing       },
        { 0x0e, &handleCharacterCreationScreen },
        { 0x0b, &handleCharacterCreation       },
    };

    while (conn.isAlive())
    {
        conn.read();
        if (auto const it = handlers.find(conn.readBuffer()[2]); it != handlers.end())
            (*it->second)(conn);
        else
            SPDLOG_INFO("Unsupported packet 0x{:02x}", conn.readBuffer()[2]);
    }
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_ERROR("Packet reading failed, disconnecting client:\n{}", l2cpp::formatExceptionStack(e));
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
    L2CPP_B_ASSERT(socketListener.listen(ip, port, &onSocketAccepted), "Failed to listen on {}:{}", ip, port);

    ioContext.run();
    return EXIT_SUCCESS;
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    return EXIT_FAILURE;
}
catch (std::exception const & e)
{
    SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    return EXIT_FAILURE;
}
catch (...)
{
    SPDLOG_CRITICAL("Unexpected top-level exception caught, terminating");
    return EXIT_FAILURE;
}
