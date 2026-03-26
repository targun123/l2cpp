/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "Move.hpp"

// Project includes
#include "../../Player.hpp"
#include "../../network/Connection.hpp"
#include "../World.hpp"
#include "../actor/Character.hpp"

#include <l2cpp/network/Packet.hpp>

MoveAction::MoveAction() noexcept
    : Action(Type::Move)
{}

bool MoveAction::canBeInterrupted() const
{
    return true;
}

void MoveAction::onStarted(Actor & actor)
{
    auto & c = static_cast<Character &>(actor);

    c.state = ActorState::Moving;
    c.setPosition(originX, originY, originZ);

    l2cpp::Network::Packet p(0x01); // Make character start moving, position will be validated in MoveUpdate handler
    p
        << c.id()
        << targetX << targetY << targetZ
        << originX << originY << originZ
    ;
    World::broadcastAround(c, std::move(p), true);
}

void MoveAction::updateImpl(ClockDuration, Actor &)
{
    setFinished(currentDistance >= totalDistance);
}
