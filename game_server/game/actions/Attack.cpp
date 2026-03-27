/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "Attack.hpp"

// Project includes
#include "../World.hpp"
#include "../../network/Connection.hpp"
#include "../../network/packets/server/combat/AttackPacket.hpp"
#include "../../network/packets/server/combat/AttackStanceTogglePacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../actor/Character.hpp"
#include "../components/AttackStanceTimer.hpp"
#include "../components/Gear.hpp"
#include "../components/Stats.hpp"

#include <l2cpp/network/Packet.hpp>

namespace SM = Network::Packet::Server;

namespace
{
    template<typename Rep, typename Period>
    auto toClockDuration(std::chrono::duration<Rep, Period> duration) -> std::chrono::steady_clock::duration
    {
        return std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration);
    }
}

AttackAction::AttackAction(Actor & target, u32 const pAtkSpeed) noexcept
    : Action(Type::Attack)
    , _target(target)
    , _hitDuration(toClockDuration(1s / (pAtkSpeed / 500.)))
    , _impactTimePoint(toClockDuration(_hitDuration * 0.55)) // Timing for 1h-sword
{}

bool AttackAction::canBeInterrupted() const
{
    return false;
}

void AttackAction::onStarted(Actor & actor)
{
    auto & c = static_cast<Character &>(actor);

    // Use soulshots if a weapon is equipped
    std::optional<ItemGrade> soulShotGrade;
    if (auto const weapon = c.gear().weapon(); weapon)
        soulShotGrade = weapon->tmplate.grade;

    World::broadcastAround(c, SM::AttackPacket(c, _target, {_target, 10, false, soulShotGrade}), true);

    c.state = ActorState::Attacking;
    c.getOrAddComponent<AttackStanceTimer>().restart();
}

void AttackAction::onFinished(Actor & actor)
{
    // Physical attacking never stops unless another action is requested (e.g. player moves) or target dies
    if (!actor.nextAction())
        actor.doNext<AttackAction>(_target, actor.stats().pAtkSpeed);
}

void AttackAction::updateImpl(ClockDuration const elapsed, Actor &)
{
    if (Utils::Chrono::thresholdCrossed(lastUpdateTime(), elapsed, _impactTimePoint))
    {
        // Enable attack stance on target once it gets hit
        World::broadcastAround(_target, SM::AttackStanceTogglePacket(true, _target), true);
        _target.getOrAddComponent<AttackStanceTimer>().restart();

        // TODO: inflict damage
    }

    setFinished(lastUpdateTime() >= startTime() + _hitDuration);
}
