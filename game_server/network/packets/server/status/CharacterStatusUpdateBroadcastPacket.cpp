/// @author    Chnossos
/// @date      Created on 2026-02-28

#include "CharacterStatusUpdateBroadcastPacket.hpp"

// Project includes
#include "../../../../game/actor/Character.hpp"
#include "../../../../game/components/Gear.hpp"
#include "../../../../game/components/CharacterStatus.hpp"
#include "../../../../game/components/Stats.hpp"
#include "../../../../game/components/Position.hpp"
#include "../../../../game/components/PlayerAppearance.hpp"

using namespace Network::Packet::Server;

CharacterStatusUpdateBroadcastPacket::CharacterStatusUpdateBroadcastPacket(Character const & c)
    : Packet(0x03)
{
    using enum GearSlot;

    *this
        << c.position().x
        << c.position().y
        << c.position().z
        << 0 // vehicleId
        << c.id()
        << c.name()
        << c.appearance().race()
        << c.appearance().sex
        << c.profession()
        << 0 // ?
        << c.gear().itemTemplateId(Head)
        << c.gear().itemTemplateId(RightHand)
        << c.gear().itemTemplateId(LeftHand)
        << c.gear().itemTemplateId(Gloves)
        << c.gear().itemTemplateId(Chest)
        << c.gear().itemTemplateId(Legs)
        << c.gear().itemTemplateId(Feet)
        << c.gear().itemTemplateId(Back)
        << 0 // c.gear().itemTemplateId(GearSlot::LeftHand)
        << c.gear().itemTemplateId(Hair)
        << 0 // (c.isPvpFlagged ? 1 : 0)
        << c.status().karma
        << c.stats().mAtkSpeed
        << c.stats().pAtkSpeed
        << 0 // (c.isPvpFlagged ? 1 : 0) // repeated
        << c.status().karma              // repeated
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
        << c.title()
        << 0     // clanId
        << 0     // clan crest id
        << 0     // alliance id
        << 0     // alliance crest id
        << 0     // ?
        << true  // standing
        << true  // running
        << c.isInCombatStance()
        << false // not in Oly, not (fake) dead
        << false // not invisible
        << 0_u8 // mount type (0=none 1=Strider 2=Wyvern)
        << 0_u8 // private store type
    ;

    *this << static_cast<u16>(c.cubics.size());
    for (auto const id : c.cubics)
        *this << id;

    *this
        << false // looking for party members
        << 0     // abnormal visual effects
        << static_cast<u8>(c.evalAmount)
        << c.evalScore
        << c.profession()
        << static_cast<u32>(c.status().cp.max)
        << static_cast<u32>(c.status().cp.current)
    ;

    auto const weapon = c.gear().weapon();
    *this << (weapon ? weapon->get().enchantLevel : 0_u8);

    *this
        << c.team()
        << 0 // clan large crest id
        << c.status().isNoble
        << c.status().isHero
        << false // fishing
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.appearance().nameColor
    ;
}
