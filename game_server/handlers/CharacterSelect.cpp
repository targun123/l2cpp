/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

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
    auto & c = player.currentCharacter().value().get();
    Packet p(0x15);
    p
        << c.name
        << c.id
        << c.title
        << player.playOk1()
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
        << std::array<u32, 53>{} // unknown
    ;
    player.connection().send(p);
}
