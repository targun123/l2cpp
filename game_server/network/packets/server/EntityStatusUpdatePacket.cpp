/// @author    Chnossos
/// @date      Created on 2026-02-28

#include "EntityStatusUpdatePacket.hpp"

// Project includes
#include "../../../game/Character.hpp"

// C++ includes
#include <array>

using namespace Network::Packet::Server;

EntityStatusUpdatePacket::EntityStatusUpdatePacket(l2::Character & c)
    : Packet(0x03)
{
    *this
        << c.pos.x
        << c.pos.y
        << c.pos.z
        << 0 // vehicleId
        << c.id
        << c.name
        << c.raceId
        << c.sex
        << c.classId
        << 0 // separator?
        << std::array<u32, 10>{} // gear
        << (c.isPvpFlagged ? 1 : 0)
        << c.karma
        << c.finalStats.mAtkSpeed
        << c.finalStats.pAtkSpeed
        << (c.isPvpFlagged ? 1 : 0) // again…
        << c.karma                  // again…
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
        << c.collisionRadius
        << c.collisionHeight
        << c.hairStyleId
        << c.hairColorId
        << c.faceId
        << c.title
        << c.clanId
        << 0     // clan crest id
        << 0     // alliance id
        << 0     // alliance crest id
        << 0     // separator
        << true  // standing
        << true  // running
        << false // in combat
        << false // not in Oly, not (fake) dead
        << false // not invisible
        << static_cast<u8>(0)  // mount type (0=none 1=Strider 2=Wyvern)
        << static_cast<u8>(0)  // private store type
    ;

    *this << static_cast<u16>(c.cubics.size());
    for (auto id : c.cubics)
        *this << id;

    *this
        << false // looking for party members
        << 0 // abnormal visual effects
        << static_cast<u8>(c.evalAmount)
        << c.evalScore
        << c.classId
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
        << false // mounted
        << c.team
        << 0 // clan large crest id
        << c.isNoble
        << c.isHero
        << false // fishing
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.nameColor
    ;
}
