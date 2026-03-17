/// @author    Chnossos
/// @date      Created on 2026-02-28

#include "CharacterStatusUpdatePacket.hpp"

// Project includes
#include <spdlog/spdlog.h>

#include "../../../../game/actor/Character.hpp"
#include "../../../../game/components/CharacterStatus.hpp"
#include "../../../../game/components/Stats.hpp"
#include "../../../../game/components/PlayerAppearance.hpp"
#include "../../../../game/components/Gear.hpp"
#include "../../../../game/inventory/ItemStorage.hpp"

using namespace Network::Packet::Server;

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
        << c.status().level()
        << c.status().xp
        << c.stats().STR
        << c.stats().DEX
        << c.stats().CON
        << c.stats().INT
        << c.stats().WIT
        << c.stats().MEN
        << static_cast<u32>(c.status().hp.max)
        << static_cast<u32>(c.status().hp.current)
        << static_cast<u32>(c.status().mp.max)
        << static_cast<u32>(c.status().mp.current)
        << c.status().sp
        << c.status().weight.current
        << c.status().weight.max
        << (c.gear().hasActiveWeapon() ? 20 : 40)
        << c.gear().itemId(Underwear)
        << c.gear().itemId(RightEar)
        << c.gear().itemId(LeftEar)
        << c.gear().itemId(Neck)
        << c.gear().itemId(RightFinger)
        << c.gear().itemId(LeftFinger)
        << c.gear().itemId(Head)
        << c.gear().itemId(RightHand)
        << c.gear().itemId(LeftHand)
        << c.gear().itemId(Gloves)
        << c.gear().itemId(Chest)
        << c.gear().itemId(Legs)
        << c.gear().itemId(Feet)
        << c.gear().itemId(Back)
        << 0 // c.gear().itemId(LeftHand)
        << c.gear().itemId(Hair)
        << c.gear().itemTemplateId(Underwear)
        << c.gear().itemTemplateId(RightEar)
        << c.gear().itemTemplateId(LeftEar)
        << c.gear().itemTemplateId(Neck)
        << c.gear().itemTemplateId(LeftFinger)
        << c.gear().itemTemplateId(RightFinger)
        << c.gear().itemTemplateId(Head)
        << c.gear().itemTemplateId(RightHand)
        << c.gear().itemTemplateId(LeftHand)
        << c.gear().itemTemplateId(Gloves)
        << c.gear().itemTemplateId(Chest)
        << c.gear().itemTemplateId(Legs)
        << c.gear().itemTemplateId(Feet)
        << c.gear().itemTemplateId(Back)
        << 0 // c.gear().itemTemplateId(LeftHand)
        << c.gear().itemTemplateId(Hair)
        << c.stats().pAtk
        << c.stats().pAtkSpeed
        << c.stats().pDef
        << c.stats().evasion
        << c.stats().accuracy
        << c.stats().pCritRate
        << c.stats().mAtk
        << c.stats().mAtkSpeed
        << c.baseStats().pAtkSpeed
        << c.stats().mDef
        << 0 // (c.isPvpFlagged ? 1 : 0)
        << c.status().karma
        << c.baseStats().runSpeed
        << c.baseStats().walkSpeed
        << c.baseStats().swimRunSpeed
        << c.baseStats().swimWalkSpeed
        << c.baseStats().flyRunSpeed
        << c.baseStats().flyWalkSpeed
        << c.baseStats().flyRunSpeed
        << c.baseStats().flyWalkSpeed
        << c.stats().moveSpeedMultiplier
        << c.stats().pAtkSpeedMultiplier
        << c.appearance().collisionRadius
        << c.appearance().collisionHeight
        << c.appearance().hairStyleId
        << c.appearance().hairColorId
        << c.appearance().faceId
        << (c.accessLevel > 0 ? 1 : 0)
        << c.title()
        << 0                  // clanId
        << 0                  // crest id
        << 0                  // ally id
        << 0                  // ally crest id
        << 0                  // clan leader: 0x60 | in-siege: 0x40 | leader rights: 0x20
        << static_cast<u8>(0) // mount type
        << static_cast<u8>(0) // private store type
        << false              // can dwarven craft
        << c.status().pkCount
        << c.status().pvpCount
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
        << c.inventory().limit()
        << c.profession()
        << 0 // special effects? circles around player...
        << static_cast<u32>(c.status().cp.max)
        << static_cast<u32>(c.status().cp.current)
    ;

    auto const weapon = c.gear().weapon();
    *this << (weapon ? weapon->get().enchantLevel : 0_u8);

    *this
        << c.team()
        << 0 // clan crest large id
        << c.status().isNoble // noble symbol in status window
        << c.status().isHero  // hero aura
        << false // is fishing
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.appearance().nameColor
    ;
}
