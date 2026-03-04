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
          << std::array<u32, 9>{}
        ;

        // constexpr size_t PAPERDOLL_UNDER = 0;
        // constexpr size_t PAPERDOLL_LEAR = 1;
        // constexpr size_t PAPERDOLL_REAR = 2;
        // constexpr size_t PAPERDOLL_NECK = 3;
        // constexpr size_t PAPERDOLL_LFINGER = 4;
        // constexpr size_t PAPERDOLL_RFINGER = 5;
        // constexpr size_t PAPERDOLL_HEAD = 6;
        // constexpr size_t PAPERDOLL_RWEAPON = 7;
        // constexpr size_t PAPERDOLL_SHIELD = 8;
        // constexpr size_t PAPERDOLL_GLOVES = 9;
        // constexpr size_t PAPERDOLL_CHEST = 10;
        // constexpr size_t PAPERDOLL_LEGS = 11;
        // constexpr size_t PAPERDOLL_FEET = 12;
        // constexpr size_t PAPERDOLL_CLOAK = 13;
        // constexpr size_t PAPERDOLL_LWEAPON = 14;
        // constexpr size_t PAPERDOLL_HAIR = 15;
        constexpr size_t PAPERDOLL_TOTALSLOTS = 16;

        /*<item id="6680" type="Armor" name="Sealed Draconic Leather Armor"
        <item id="6681" type="Armor" name="Sealed Draconic Leather Glove"
        <item id="6682" type="Armor" name="Sealed Draconic Leather Boots"
        <item id="6683" type="Armor" name="Sealed Draconic Leather Helmet"*/

        std::array<u32, PAPERDOLL_TOTALSLOTS> gearUids{};
        std::array<u32, PAPERDOLL_TOTALSLOTS> gearIds{};
        // gearIds[PAPERDOLL_HEAD] = 6683;
        // gearIds[PAPERDOLL_RWEAPON] = 6600;
        // gearIds[PAPERDOLL_LWEAPON] = 6600;
        // gearIds[PAPERDOLL_SHIELD] = 6678;
        // gearIds[PAPERDOLL_GLOVES] = 6681;
        // gearIds[PAPERDOLL_CHEST] = 6680;
        // gearIds[PAPERDOLL_FEET] = 6682;
        p << gearUids
          << gearIds
        ;

        p << c.hairStyleId
          << c.hairColorId
          << c.faceId
          << c.hp.max
          << c.mp.max
          << c.deleteTime
          << c.classId
          << c.selected
          << 0_u8 // weapon enchant level (max 127)
        ;
    }

    player.connection().send(p);
}
