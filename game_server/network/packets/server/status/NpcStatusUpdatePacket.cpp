/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "NpcStatusUpdatePacket.hpp"

// Project includes
#include "../../../../game/actor/NonPlayableActor.hpp"
#include "../../../../game/components/NpcAppearance.hpp"
#include "../../../../game/components/Stats.hpp"

using Network::Packet::Server::NpcStatusUpdatePacket;

NpcStatusUpdatePacket::NpcStatusUpdatePacket(NonPlayableActor const & actor)
    : Packet(0x16)
{
    *this
        << actor.id()
        << actor.appearance().id()
        << (actor.isAttackable() ? 1 : 0)
        << actor.position().x
        << actor.position().y
        << actor.position().z
        << actor.appearance().headAngle
        << 0 // ?
        << actor.stats().mAtkSpeed
        << actor.stats().pAtkSpeed
        << actor.stats().runSpeed
        << actor.stats().walkSpeed
        << actor.stats().swimRunSpeed
        << actor.stats().swimWalkSpeed
        << actor.stats().flyRunSpeed
        << actor.stats().flyWalkSpeed
        << actor.stats().flyRunSpeed
        << actor.stats().flyWalkSpeed
        << actor.stats().moveSpeedMultiplier
        << actor.stats().pAtkSpeedMultiplier
        << actor.appearance().collisionRadius
        << actor.appearance().collisionHeight
        << 0 // right hand
        << 0 // chest
        << 0 // left hand
        << actor.appearance().nameIsVisible()
        << true // isRunning
        << actor.isInCombatStance()
        << false // is like dead
        << false // isSummoned
        << actor.name()
        << actor.title()
        << actor.appearance().titleColor()
        << 0 // not pvp flagged
        << 0 // no karma
        << 0 // no visual effect
        << 0 // no clan
        << 0 // no clan crest
        << 0 // no ally
        << 0 // no ally crest
        << false // not in water nor flying
        << actor.team()
        << actor.appearance().collisionRadius
        << actor.appearance().collisionHeight
        << 0 // weapon enchant level
    ;
}
