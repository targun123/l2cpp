/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "MoveAction.hpp"

// Project includes
#include "../../Player.hpp"
#include "../../network/Connection.hpp"
#include "../World.hpp"
#include "../actor/Character.hpp"

#include <l2cpp/network/Packet.hpp>

MoveAction::MoveAction(Actor & performer, Position const & origin, Position const & target, Input const input)
    : Action(ActionType::Move, performer)
    , _origin(origin)
    , _target(target)
    , _input(input)
{}

bool MoveAction::canBeInterruptedByAnotherAction() const
{
    return true;
}

void MoveAction::onStarted()
{
    performer().state = ActorState::Moving;
    performer().setPosition(_origin);

    l2cpp::Network::Packet p(0x01); // Make character start moving, position will be validated in MoveUpdate handler
    p
        << performer().id()
        << _target
        << _origin
    ;
    World::broadcastAround(performer(), std::move(p), true);
}

void MoveAction::updateImpl(ClockDuration)
{
    setFinished(_currentDistance >= _totalDistance);
}
