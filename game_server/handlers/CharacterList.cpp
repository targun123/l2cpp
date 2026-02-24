/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(CharacterList)
{
    Packet p(0x13);
    p << static_cast<u32>(player.characters().size());

    for (auto const & c : player.characters())
    {
        p << c.name
          << c.id
          << player.accountName()
          << player.playOk1()
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

    player.connection().send(p);
}
