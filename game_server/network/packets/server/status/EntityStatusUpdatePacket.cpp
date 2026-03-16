/// @author    Chnossos
/// @date      Created on 2026-02-28

#include "EntityStatusUpdatePacket.hpp"

// Project includes
#include "../../../../game/actor/Character.hpp"
#include "../../../../game/inventory/Gear.hpp"
#include "../../../../game/components/Stats.hpp"
#include "../../../../game/components/PlayerAppearance.hpp"

using namespace Network::Packet::Server;

EntityStatusUpdatePacket::EntityStatusUpdatePacket(Character const & c)
    : Packet(0x03)
{
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
        << 0 // unknown
    ;

    auto const headItem = c.gear().item(GearSlot::Head);
    *this << (headItem ? headItem->get().tmplate.id : 0);

    *this
        << 0 // (c.isPvpFlagged ? 1 : 0)
        << 0 // karma
        << c.stats().mAtkSpeed
        << c.stats().pAtkSpeed
        << 0 // (c.isPvpFlagged ? 1 : 0) // repeated
        << 0 // karma                    // repeated
        << c.baseStats().runSpeed
        << c.baseStats().walkSpeed
        << c.baseStats().swimRunSpeed
        << c.baseStats().swimWalkSpeed
        << c.baseStats().flyRunSpeed
        << c.baseStats().flyWalkSpeed
        << c.baseStats().flyRunSpeed
        << c.baseStats().flyWalkSpeed
        << c.stats().moveSpeedMutliplier
        << c.stats().pAtkSpeedMutliplier
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
        << 0     // separator
        << true  // standing
        << true  // running
        << c.isInCombatStance()
        << false // not in Oly, not (fake) dead
        << false // not invisible
        << static_cast<u8>(0) // mount type (0=none 1=Strider 2=Wyvern)
        << static_cast<u8>(0) // private store type
    ;

    *this << static_cast<u16>(c.cubics.size());
    for (auto const id : c.cubics)
        *this << id;

    *this
        << false  // looking for party members
        << 0      // abnormal visual effects
        << static_cast<u8>(c.evalAmount)
        << c.evalScore
        << c.profession()
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
        << false // mounted
        << c.team()
        << 0 // clan large crest id
        << false // c.isNoble
        << false // c.isHero
        << false // fishing
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.appearance().nameColor
    ;
}
