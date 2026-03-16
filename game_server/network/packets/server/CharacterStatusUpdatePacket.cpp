/// @author    Chnossos
/// @date      Created on 2026-02-28

#include "CharacterStatusUpdatePacket.hpp"

// Project includes
#include "../../../game/actor/Character.hpp"
#include "../../../game/components/PlayerAppearance.hpp"
#include "../../../game/inventory/Gear.hpp"

using namespace Network::Packet::Server;

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

CharacterStatusUpdatePacket::CharacterStatusUpdatePacket(Character & c)
    : Packet(0x04)
{
    using enum GearSlot;

    *this
        << c.position().x
        << c.position().y
        << c.position().z
        << c.appearance().headAngle
        << c.id()
        << c.name()
        << c.appearance().race()
        << c.appearance().sex
        << c.profession()
        << c.level
        << c.xp
        << c.finalStats.STR
        << c.finalStats.DEX
        << c.finalStats.CON
        << c.finalStats.INT
        << c.finalStats.WIT
        << c.finalStats.MEN
        << static_cast<u32>(c.hp.max)
        << static_cast<u32>(c.hp.current)
        << static_cast<u32>(c.mp.max)
        << static_cast<u32>(c.mp.current)
        << c.sp
        << c.weight.current
        << c.weight.max
        << (c.gear().hasActiveWeapon() ? 20 : 40)
        << gearItemIdIfValid(c, Underwear)
        << gearItemIdIfValid(c, RightEar)
        << gearItemIdIfValid(c, LeftEar)
        << gearItemIdIfValid(c, Neck)
        << gearItemIdIfValid(c, RightFinger)
        << gearItemIdIfValid(c, LeftFinger)
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
        << gearItemTemplateIdIfValid(c, RightEar)
        << gearItemTemplateIdIfValid(c, LeftEar)
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
        << c.finalStats.pAtk
        << c.finalStats.pAtkSpeed
        << c.finalStats.pDef
        << c.finalStats.evasion
        << c.finalStats.accuracy
        << c.finalStats.pCritRate
        << c.finalStats.mAtk
        << c.finalStats.mAtkSpeed
        << c.baseStats.pAtkSpeed
        << c.finalStats.mDef
        << (c.isPvpFlagged ? 1 : 0)
        << c.karma
        << c.baseStats.runSpeed
        << c.baseStats.walkSpeed
        << c.baseStats.swimRunSpeed
        << c.baseStats.swimWalkSpeed
        << c.baseStats.flyRunSpeed
        << c.baseStats.flyWalkSpeed
        << c.baseStats.flyRunSpeed
        << c.baseStats.flyWalkSpeed
        << c.finalStats.moveSpeedMutliplier
        << c.finalStats.pAtkSpeedMutliplier
        << c.appearance().collisionRadius
        << c.appearance().collisionHeight
        << c.appearance().hairStyleId
        << c.appearance().hairColorId
        << c.appearance().faceId
        << (c.accessLevel > 0 ? 1 : 0)
        << c.title()
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

    *this << static_cast<u16>(c.cubics.size());
    for (auto const id : c.cubics)
        *this << id;

    *this
        << false // looking for party members
        << 0     // abnormal effects
        << false // is inside water
        << 0     // clan privileges
        << 0     // 0x100 - swim?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << c.evalAmount
        << c.evalScore
        << 0 // ?
        << c.inventoryLimit
        << c.profession()
        << 0 // special effects? circles around player...
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
    ;

    auto const weapon = c.gear().weapon();
    *this << (weapon ? weapon->get().enchantLevel : 0_u8);

    *this
        << c.team()
        << 0 // clan crest large id
        << c.isNoble // noble symbol in status window
        << c.isHero  // hero aura
        << false // is fishing
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.appearance().nameColor
    ;
}
