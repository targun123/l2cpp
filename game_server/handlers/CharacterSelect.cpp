/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/CharacterStatus.hpp"
#include "../game/components/Stats.hpp"
#include "../game/components/PlayerAppearance.hpp"

DEFINE_PACKET_HANDLER(CharacterSelect)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 index;
    reader >> index;

    if (index >= player.characters().size())
    {
        SPDLOG_ERROR("Invalid character selection index: {}", index);
        index = 0;
    }

    player.setCurrentCharacter(index);
    auto const & c = player.currentCharacter()->get();
    Packet p(0x15);
    p
        << c.name()
        << c.id()
        << c.title()
        << player.playOk1()
        << 0 // clanId
        << 0 // ?
        << c.appearance().sex
        << c.appearance().race()
        << c.profession()
        << 1 // active (?)
        << c.position().x
        << c.position().y
        << c.position().z
        << c.status().hp.current
        << c.status().mp.current
        << c.status().sp
        << c.status().xp
        << c.status().level()
        << c.status().karma
        << c.status().pkCount
        << c.baseStats().INT
        << c.baseStats().STR
        << c.baseStats().CON
        << c.baseStats().MEN
        << c.baseStats().DEX
        << c.baseStats().WIT
        << std::array<u32, 53>{} // ?
    ;
    player.connection().send(p);
}
