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
    reader >> gApp.userName >> playOk2 >> gApp.playOk1 >> loginOk1 >> loginOk2;
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

static void handleManorList(Connection & conn)
{
    conn.send(Packet(0xfe).append<u16>(0x1b) << 0);
}

static void handleQuestList(Connection & conn)
{
    constexpr u16 questAmount = 0, questItems = 0;
    conn.send(Packet(0x80) << questAmount << questItems);
}

static void handleUserInfo(Connection & conn)
{
    auto & c = gApp.characters.front();

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
        << std::array<u32, 32>{} // gear
        << c.stats.pAtk
        << c.stats.pAtkSpeed
        << c.stats.pDef
        << c.stats.evasion
        << c.stats.accuracy
        << c.stats.critRate
        << c.stats.mAtk
        << c.stats.mAtkSpeed
        << c.stats.pAtkSpeed
        << c.stats.mDef
        << (c.isPvpFlagged ? 1 : 0)
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
        << c.collisionRadius
        << c.collisionHeight
        << c.hairStyleId
        << c.hairColorId
        << c.faceId
        << (c.accessLevel > 0 ? 1 : 0)
        << c.title
        << c.clanId
        << 0 // crest id
        << 0 // ally id
        << 0 // ally crest id
        << 0 // clan leader: 0x60 | in-siege: 0x40 | leader rights: 0x20
        << static_cast<u8>(0) // mount type
        << static_cast<u8>(0) // private store type
        << false // can dwarven craft
        << c.pkCount
        << c.pvpCount
    ;

    p << static_cast<u16>(c.cubics.size());
    for (auto id : c.cubics)
        p << id;

    p
        << false // looking for party members
        << 0 // abnormal effects
        << static_cast<u8>(0) // ?
        << 0 // clan privileges
        << 0 // 0x100 // swim
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << c.evalAmount
        << c.evalScore
        << 0 // ?
        << c.inventoryLimit
        << c.classId
        << 0 // special effects? circles around player...
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
        << c.enchantEffect
        << c.team // duel team color: 0=none 1=blue 2=red
        << 0 // clan crest large id
        << c.isHero // hero symbol in status window
        << c.isHero // hero aura
        << static_cast<u8>(0) // fishing mode (unused)
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.nameColor
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
        HANDLER(0xd0, ManorList),
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
            SPDLOG_WARN("Unsupported packet 0x{:02x}", conn.readBuffer()[2]);
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
