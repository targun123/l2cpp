/// @author    Chnossos
/// @date      Created on 2026-03-21

#include "AttackPacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"
#include "../../../../game/components/Gear.hpp"
#include "../../../../game/components/Position.hpp"

using Network::Packet::Server::AttackPacket;

AttackPacket::AttackPacket(Actor const & attacker, Actor const & mainTarget)
    : Packet(0x05)
    , _attacker(attacker)
    , _mainTarget(mainTarget)
{
    *this
        << attacker.id()
    ;
}

void AttackPacket::addHit(Hit hit)
{
    // TODO: check that this specific hit's target does not already have a hit recorded
    _hits.emplace_back(std::move(hit));
}

void AttackPacket::finalizeImpl()
{
    auto const mainTarget = _hits.cbegin();

    *this
        << *mainTarget
        << _attacker.position().x
        << _attacker.position().y
        << _attacker.position().z
        << static_cast<u16>(_hits.size() - 1)
    ;

    for (auto it = mainTarget + 1; it != _hits.cend(); ++it)
        *this << *it;

    *this
        << _mainTarget.position().x
        << _mainTarget.position().y
        << _mainTarget.position().z
    ;
}
