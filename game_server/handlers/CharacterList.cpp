/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

namespace
{
    u32 gearItemIdIfValid(l2::Character const & c, l2::GearSlot const s)
    {
        auto const item = c.gearItem(s);
        return item ? item->get().id() : 0;
    };

    u32 gearItemTemplateIdIfValid(l2::Character const & c, l2::GearSlot const s)
    {
        auto const item = c.gearItem(s);
        return item ? item->get().tmplate.id : 0;
    };
}

DEFINE_PACKET_HANDLER(CharacterList)
{
    Packet p(0x13);
    p << static_cast<u32>(player.characters().size());

    using enum l2::GearSlot;
    for (auto const & c : player.characters())
    {
        p
            << c.name
            << c.id()
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
            << std::array<u32, 9>{} // unknown
            << gearItemIdIfValid(c, Underwear)
            << gearItemIdIfValid(c, LeftEar)
            << gearItemIdIfValid(c, RightEar)
            << gearItemIdIfValid(c, Neck)
            << gearItemIdIfValid(c, LeftFinger)
            << gearItemIdIfValid(c, RightFinger)
            << gearItemIdIfValid(c, Head)
            << gearItemIdIfValid(c, RightHand)
            << gearItemIdIfValid(c, LeftHand)
            << gearItemIdIfValid(c, Gloves)
            << gearItemIdIfValid(c, Chest)
            << gearItemIdIfValid(c, Legs)
            << gearItemIdIfValid(c, Feet)
            << gearItemIdIfValid(c, Back)
            << 0 // gearItemIdIfValid(c, LeftHand)
            << gearItemIdIfValid(c, Hair)
            << gearItemTemplateIdIfValid(c, Underwear)
            << gearItemTemplateIdIfValid(c, LeftEar)
            << gearItemTemplateIdIfValid(c, RightEar)
            << gearItemTemplateIdIfValid(c, Neck)
            << gearItemTemplateIdIfValid(c, LeftFinger)
            << gearItemTemplateIdIfValid(c, RightFinger)
            << gearItemTemplateIdIfValid(c, Head)
            << gearItemTemplateIdIfValid(c, RightHand)
            << gearItemTemplateIdIfValid(c, LeftHand)
            << gearItemTemplateIdIfValid(c, Gloves)
            << gearItemTemplateIdIfValid(c, Chest)
            << gearItemTemplateIdIfValid(c, Legs)
            << gearItemTemplateIdIfValid(c, Feet)
            << gearItemTemplateIdIfValid(c, Back)
            << 0 // gearItemTemplateIdIfValid(c, LeftHand)
            << gearItemTemplateIdIfValid(c, Hair)
            << c.hairStyleId
            << c.hairColorId
            << c.faceId
            << c.hp.max
            << c.mp.max
            << c.deleteTime
            << c.classId
            << c.selected
        ;

        auto const weapon = c.weapon();
        p << (weapon ? weapon->get().enchantLevel : 0_u8);
    }

    player.connection().send(p);
}
