/// @author    Chnossos
/// @date      Created on 2026-03-25

#include "ActorAttackStanceTimerSystem.hpp"

// Project includes
#include "../../Player.hpp"
#include "../../network/Connection.hpp"
#include "../../network/packets/server/combat/AttackStanceTogglePacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../actor/Character.hpp"
#include "../components/AttackStanceTimer.hpp"

#include <spdlog/spdlog.h>

namespace
{
    constexpr ClockDuration attackStanceTimeout = 5s;
}

void ActorAttackStanceTimerSystem::updateImpl(ClockDuration const elapsed, Actor & actor)
{
    if (OptRef const timer = actor.component<AttackStanceTimer>())
    {
        if (Utils::Chrono::thresholdCrossed(timer->elapsedSinceStart, elapsed, attackStanceTimeout))
        {
            if (actor.state == ActorState::CombatIdle)
                actor.state = ActorState::Idle;

            if (actor.type() == ActorType::Character)
            {
                auto const & c = static_cast<Character const &>(actor);
                c.player->connection().send(Network::Packet::Server::AttackStanceTogglePacket(false, c));
            }
            else
                SPDLOG_DEBUG(L"Actor '{}' attack stance off", actor.name()); // broadcast around
        }
        timer->elapsedSinceStart += elapsed;
    }
}
