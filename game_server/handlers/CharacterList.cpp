/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/PlayerAppearance.hpp"
#include "../game/inventory/Gear.hpp"

namespace
{
    u32 gearItemIdIfValid(Character const & c, GearSlot const s)
    {
        auto const item = c.gear().item(s);
        return item ? item->get().id() : 0;
    }

    u32 gearItemTemplateIdIfValid(Character const & c, GearSlot const s)
    {
        auto const item = c.gear().item(s);
        return item ? item->get().tmplate.id : 0;
    }
}

DEFINE_PACKET_HANDLER(CharacterList)
{
    Packet p(0x13);
    p << static_cast<u32>(player.characters().size());

    using enum GearSlot;
    for (auto const & c : player.characters())
    {
        p
            << c.name()
            << c.id()
            << player.accountName()
            << player.playOk1()
            << c.clanId
            << 0
            << c.appearance().sex
            << c.appearance().race()
            << c.profession()
            << 1 // active
            << c.position().x
            << c.position().y
            << c.position().z
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
            << c.appearance().hairStyleId
            << c.appearance().hairColorId
            << c.appearance().faceId
            << c.hp.max
            << c.mp.max
            << c.deleteTime
            << c.profession()
            << c.selected
        ;

        auto const weapon = c.gear().weapon();
        p << (weapon ? weapon->get().enchantLevel : 0_u8);
    }

    player.connection().send(p);
}
