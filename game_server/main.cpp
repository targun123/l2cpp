/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include "game/Character.hpp"
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

class PacketReader
{
public:
    explicit PacketReader(std::span<byte const> packet) noexcept: cursor(std::move(packet)) {}

public:
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
    PacketReader & operator>>(T & t)
    {
        std::memcpy(&t, cursor.data(), sizeof(t));
        cursor = cursor.subspan(sizeof(T));
        return *this;
    }

    template<typename C>
    PacketReader & operator>>(std::basic_string<C> & str)
    {
        str = reinterpret_cast<C const *>(cursor.data());
        cursor = cursor.subspan(str.size() * sizeof(C) + sizeof(C));
        return *this;
    }

private:
    std::span<byte const> cursor;
};

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
          << c.posX // x
          << c.posY // y
          << c.posZ // z
          << c.currentHp
          << c.currentMp
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
          << c.maxHp
          << c.maxMp
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
    p << c.name
      << c.id
      << L"l2cpp" // title
      << gApp.playOk1
      << c.clanId
      << 0
      << c.sex
      << c.raceId
      << c.classId
      << 1 // active
      << c.posX // x
      << c.posY // y
      << c.posZ // z
      << c.currentHp
      << c.currentMp
      << c.sp
      << c.xp
      << c.level
      << c.karma
      << c.pkCount
      << 10 // INT
      << 10 // STR
      << 10 // CON
      << 10 // MEN
      << 10 // DEX
      << 10 // WIT
    ;

    constexpr std::array<u32, 53> unknown{};
    p << unknown;

    conn.send(p);
}

static void handleAutoShots(Connection & conn)
{
    conn.send(Packet(0x1b).append<u16>(0) << 0);
}

static void handleEnterWorld(Connection & conn)
{
    auto & c = gApp.characters.front();

    // AKA UserInfo packet
    Packet p(0x04);
    p
        << c.posX // x
        << c.posY // y
        << c.posZ // z
        << 0 // heading
        << c.id
        << c.name
        << c.raceId
        << c.sex
        << c.classId
        << c.level
        << c.xp
        << 10 // STR
        << 10 // DEX
        << 10 // CON
        << 10 // INT
        << 10 // WIT
        << 10 // MEN
        << c.maxHp
        << c.currentHp
        << c.maxMp
        << c.currentMp
        << c.sp
        << 0 // current weight
        << 0 // mac weight
        << 0x28 // ?
    ;

    constexpr std::array<u32, 32> unknown{};
    p << unknown;

    p
        << 0 // p_atk,
        << 0 // p_atk_speed,
        << 0 // p_def,
        << 0 // evasion_rate,
        << 0 // accuracy,
        << 0 // critical_hit,
        << 0 // m_atk,
        << 0 // m_atk_speed,
        << 0 // p_atk_speed,
        << 0 // m_def,
        << 0 // pvp_flag,
        << 0 // karma,
        << 180 // run_speed,
        << 100 // walk_speed,
        << 0 // swim_run_speed,
        << 0 // swim_walk_speed,
        << 0 // fly_run_speed,
        << 0 // fly_walk_speed,
        << 0 // fly_run_speed,
        << 0 // fly_walk_speed,
        << 1.0 // movement_speed_multiplier,
        << 1.0 // atk_speed_multiplier,
        << 20.0 // collision_radius,
        << 20.0 // collision_height,
        << c.hairStyleId
        << c.hairColorId
        << c.faceId
        << 1 // access level
        << L"l2cpp" // title
        << c.clanId
        << 0 // crest id
        << 0 // ally id
        << 0 // ally crest id
        << 0 // clan leader
        << u8(0) // mount type
        << u8(0) // private store type
        << u8(0) // dwarven craft
        << c.pkCount
        << c.pvpCount
        << u16(0) // cubics
        << u8(0) // party members
        << 0 // abnormal effects
        << u8(0) // ?
        << 0 // clan privileges
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << u16(0) // recommendations left
        << u16(0) // recommendations have
        << 0 // ?
        << u16(0) // inventory limit
        << c.classId
        << 0 // ?
        << 1000 // max CP
        << 1000 // cur CP
        << false // mounted
        << u8(0) // duel color: 1=blue 2=red
        << 0 // clan crest large id
        << u8(0) // hero symbol
        << u8(0) // hero
        << u8(0) // ?
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << 0xFFFFFF // name color
    ;

    conn.send(p);
}

static void handleCancelCharacterDeletion(Connection & conn)
{
    PacketReader reader(conn.readBuffer().subspan(3));

    u32 characterId;
    reader >> characterId;

    auto const it = std::ranges::find_if(gApp.characters, [=] (auto const & c) { return c.id == characterId; });
    if (it != gApp.characters.cend())
        it->deleteTime = 0;

    handleCharacterList(conn);
}

void onSocketAccepted(boost::asio::ip::tcp::socket && socket) try
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
        { 0x0d, &handleSelectCharacter         },
        { 0xd0, &handleAutoShots               },
        { 0x03, &handleEnterWorld              },
        { 0x62, &handleCancelCharacterDeletion },
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
