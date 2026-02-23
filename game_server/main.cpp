/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include "game/Character.hpp"
#include "network/Connection.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/PacketReader.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

// C++ includes
#include <iostream>

using l2cpp::Network::PacketReader;

struct
{
    std::wstring userName;
    u32 playOk1;
    std::vector<Character> characters;
} gApp;

void onSocketAccepted(boost::asio::ip::tcp::socket && socket);

static void handleProtocol(Connection & conn)
{
    PacketReader reader(conn.readBuffer().subspan(3));

    u32 protocol;
    reader >> protocol;

    SPDLOG_INFO("Client protocol: {}", protocol);
    conn.send(Packet(0x00).append<u8>(1) << conn.encryptionKey(), false);
}

static void handleAuth(Connection const & conn)
{
    PacketReader reader(conn.readBuffer().subspan(3));

    u32 playOk2, loginOk1, loginOk2;
    reader >> gApp.userName >> gApp.playOk1 >> playOk2 >> loginOk1 >> loginOk2;
    SPDLOG_DEBUG(L"'{}' | {} | {} | {} | {}", gApp.userName, loginOk1, loginOk2, gApp.playOk1, playOk2);
}

static void handleConnectionClosing(Connection & conn)
{
    if (!gApp.characters.empty())
        conn.send(Packet() << 0x7e);

    conn.close();
}

static void handleCharacterList(Connection & conn)
{
    if (gApp.characters.empty())
    {
        gApp.characters.emplace_back();
    }

    Packet p(0x13);
    p << static_cast<u32>(gApp.characters.size());

    for (auto const & c : gApp.characters)
    {
        p << c.name
          << c.id
          << gApp.userName
          << gApp.playOk1
          << c.clanId
          << 0
          << c.sex
          << c.raceId
          << c.classId
          << 1 // active
          << c.pos.x
          << c.pos.y
          << c.pos.z
          << c.hp.current
          << c.mp.current
          << c.sp
          << c.xp
          << c.level
          << c.karma
          << c.pkCount
          // << c.pvpCount
        ;

        constexpr std::array<u32, 40> unknown{};
        p << unknown;

        p << c.hairStyleId
          << c.hairColorId
          << c.faceId
          << c.hp.max
          << c.mp.max
          << c.deleteTime
          << c.classId
          << c.selected
          << c.enchantEffect
          // << c.augmentationId
          // << c.transformationId
        ;
    }

    conn.send(p);
}

static void handleCharacterCreationScreen(Connection & conn)
{
    conn.send(Packet(0x17) << 0);
}

static void handleCharacterCreation(Connection & conn)
{
    PacketReader reader(conn.readBuffer().subspan(3));

    u32 ignore;

    auto & c = gApp.characters.emplace_back();
    reader >> c.name >> c.raceId >> c.sex >> c.classId >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
           >> c.hairStyleId >> c.hairColorId >> c.faceId;
    c.selected = 1;

    SPDLOG_DEBUG(L"Character name: '{}', race: {}, sex: {}, class: {}", c.name, c.raceId, c.sex, c.classId);

    conn.send(Packet(0x19) << 1);
    handleCharacterList(conn);
}

static void handleSelectCharacter(Connection & conn)
{
    PacketReader reader(conn.readBuffer().subspan(3));

    u32 id;
    reader >> id;

    auto & c = gApp.characters.front();

    Packet p(0x15);
    p
        << c.name
        << c.id
        << c.title
        << gApp.playOk1
        << c.clanId
        << 0 // unknown
        << c.sex
        << c.raceId
        << c.classId
        << 1 // active
        << c.pos.x
        << c.pos.y
        << c.pos.z
        << c.hp.current
        << c.mp.current
        << c.sp
        << c.xp
        << c.level
        << c.karma
        << c.pkCount
        << c.attributes.INT
        << c.attributes.STR
        << c.attributes.CON
        << c.attributes.MEN
        << c.attributes.DEX
        << c.attributes.WIT
    ;

    constexpr std::array<u32, 53> unknown{};
    p << unknown;

    conn.send(p);
}

static void handleCancelCharacterDeletion(Connection & conn)
{
    PacketReader reader(conn.readBuffer().subspan(3));

    u32 characterId;
    reader >> characterId;

    // ReSharper disable once CppTooWideScopeInitStatement
    auto const it = std::ranges::find_if(gApp.characters, [=] (auto const & c) { return c.id == characterId; });
    if (it != gApp.characters.cend())
        it->deleteTime = 0;

    handleCharacterList(conn);
}

static void handleAutoShots(Connection & conn)
{
    conn.send(Packet(0x1b).append<u16>(0) << 0);
}

static void handleQuestList(Connection & conn)
{
    constexpr u16 questAmount = 0, questItems = 0;
    conn.send(Packet(0x80) << questAmount << questItems);
}

static void handleUserInfo(Connection & conn)
{
    auto & c = gApp.characters.front();

    // AKA UserInfo packet
    Packet p(0x04);
    p
        << c.pos.x
        << c.pos.y
        << c.pos.z
        << 0 // head angle
        << c.id
        << c.name
        << c.raceId
        << c.sex
        << c.classId
        << c.level
        << c.xp
        << c.attributes.STR
        << c.attributes.DEX
        << c.attributes.CON
        << c.attributes.INT
        << c.attributes.WIT
        << c.attributes.MEN
        << static_cast<u32>(c.hp.max)
        << static_cast<u32>(c.hp.current)
        << static_cast<u32>(c.mp.max)
        << static_cast<u32>(c.mp.current)
        << c.sp
        << c.weight.current
        << c.weight.max
        << 0x28 // ?
    ;

    constexpr std::array<u32, 32> unknown{};
    p << unknown;

    p
        << c.stats.pAtk
        << c.stats.pAtkSpeed
        << c.stats.pDef
        << c.stats.evasion
        << c.stats.accuracy
        << c.stats.critRate
        << c.stats.mAtk
        << c.stats.castSpeed
        << c.stats.pAtkSpeed
        << c.stats.mDef
        << 0 // pvp_flag,
        << c.karma
        << c.stats.runSpeed
        << c.stats.walkSpeed
        << c.stats.swimRunSpeed
        << c.stats.swimWalkSpeed
        << c.stats.flyRunSpeed
        << c.stats.flyWalkSpeed
        << c.stats.flyRunSpeed
        << c.stats.flyWalkSpeed
        << 1.0 // movement_speed_multiplier,
        << 1.0 // atk_speed_multiplier,
        << 20.0 // collision_radius,
        << 100.0 // collision_height,
        << c.hairStyleId
        << c.hairColorId
        << c.faceId
        << 1 // access level
        << c.title
        << c.clanId
        << 0 // crest id
        << 0 // ally id
        << 0 // ally crest id
        << 0 // clan leader
        << static_cast<u8>(0) // mount type
        << static_cast<u8>(0) // private store type
        << static_cast<u8>(0) // dwarven craft
        << c.pkCount
        << c.pvpCount
        << static_cast<u16>(0) // cubics
        << false // looking for party members
        << 0 // abnormal effects
        << static_cast<u8>(0) // ?
        << 0 // clan privileges
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << static_cast<u16>(42) // recommendations left
        << static_cast<u16>(100) // recommendations have
        << 0 // ?
        << static_cast<u16>(1000) // inventory limit
        << c.classId
        << 0 // ?
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
        << false // mounted
        << static_cast<u8>(0) // duel color: 1=blue 2=red
        << 0 // clan crest large id
        << static_cast<u8>(0) // hero symbol
        << static_cast<u8>(0) // hero
        << static_cast<u8>(0) // ?
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << 0xFFFFFF // name color
    ;

    conn.send(p);
}

static void handleLeaveWorld(Connection & conn)
{
    conn.send(Packet(0x5f) << 1);
    handleCharacterList(conn);
}

void onSocketAccepted(boost::asio::ip::tcp::socket && socket) try
{
    Connection conn(std::move(socket));
    conn.read(false); // First packet is not encrypted
    auto size = *reinterpret_cast<u16 const *>(conn.readBuffer().data());
    std::cout << l2cpp::hexdump(conn.readBuffer().data(), size) << std::endl;
    handleProtocol(conn);

    conn.read();
    size = *reinterpret_cast<u16 const *>(conn.readBuffer().data());
    std::cout << l2cpp::hexdump(conn.readBuffer().data(), size) << std::endl;
    handleAuth(conn);
    handleCharacterList(conn);

    using PacketHandler = void(*)(Connection & conn);
    std::unordered_map<byte, std::pair<PacketHandler, std::string_view>> const handlers
    {
#define HANDLER(opCode, name) { opCode, {&handle ## name, #name} }
        HANDLER(0x09, ConnectionClosing),
        HANDLER(0x0e, CharacterCreationScreen),
        HANDLER(0x0b, CharacterCreation),
        HANDLER(0x0d, SelectCharacter),
        HANDLER(0x62, CancelCharacterDeletion),
        HANDLER(0xd0, AutoShots),
        HANDLER(0x63, QuestList),
        HANDLER(0x03, UserInfo),
        HANDLER(0x46, LeaveWorld),
#undef HANDLER
    };

    while (conn.isAlive())
    {
        conn.read();
        if (auto const it = handlers.find(conn.readBuffer()[2]); it != handlers.end())
        {
            auto const & [handler, handlerName] = it->second;
            SPDLOG_DEBUG("-> {}", handlerName);
            size = *reinterpret_cast<u16 const *>(conn.readBuffer().data());
            std::cout << l2cpp::hexdump(conn.readBuffer().data(), size) << std::endl;
            (*handler)(conn);
        }
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
