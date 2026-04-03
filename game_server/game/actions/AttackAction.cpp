/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "AttackAction.hpp"

// Project includes
#include "../../network/packets/server/combat/AttackPacket.hpp"
#include "../../network/packets/server/combat/AttackStanceTogglePacket.hpp"
#include "../../network/packets/server/status/ActorDiePacket.hpp"
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../World.hpp"
#include "../actor/Character.hpp"
#include "../components/AttackStanceTimer.hpp"
#include "../components/Gear.hpp"
#include "../components/Stats.hpp"

#include <l2cpp/network/Packet.hpp>

namespace SM = Network::Packet::Server;

AttackAction::AttackAction(Actor & target, u32 const pAtkSpeed) noexcept
    : Action(ActionType::Attack)
    , _target(target)
    , _hitDuration(Utils::Chrono::Clock::toDuration(1s / (pAtkSpeed / 500.)))
{}

bool AttackAction::canBeInterrupted() const { return false; }

void AttackAction::onStarted(Actor & actor)
{
    actor.state = ActorState::Attacking;

    u8 hitCount = 1;

    std::optional<ItemGrade> soulShotGrade;
    if (actor.type() == ActorType::Character)
    {
        auto & c = static_cast<Character &>(actor);

        if (auto const weapon = c.gear().weapon())
        {
            // Use soulshots if a weapon is equipped
            soulShotGrade = weapon->tmplate.grade;

            // Dual weapon means same item in both hands
            if (auto const id = c.gear().itemId(GearSlot::LeftHand); id && weapon->id() == id)
                ++hitCount;
        }
        else if (!c.gear().item(GearSlot::LeftHand))
            ++hitCount; // bare fists have two hits when not carrying a shield
    }

    SM::AttackPacket p(actor, _target);
    for (decltype(hitCount) i = 0; i < hitCount; ++i) // split dual hits damage
        p.addHit({_target, 50u / hitCount, false, soulShotGrade});

    World::broadcastAround(actor, std::move(p), true);
}

void AttackAction::updateImpl(ClockDuration const, Actor &)
{
    setFinished(lastUpdateTime() >= startTime() + _hitDuration);
}

void AttackAction::onFinished(Actor & actor)
{
    // Enable attack stance on target once it gets hit
    World::broadcastAround(_target, SM::AttackStanceTogglePacket(true, _target), true);

    actor.getOrAddComponent<AttackStanceTimer>().restart();
    _target.getOrAddComponent<AttackStanceTimer>().restart();

    bool targetIsDead = false;

    auto & stats = *_target.component<ComputedStats>();
    if ((stats.curHp -= 100) <= 0)
    {
        stats.curHp = 0;
        targetIsDead = true;
    }

    Network::Packet::Server::StatsUpdatePacket p(_target);
    p.addStat(Stat::CurHp, static_cast<u32>(stats.curHp));
    World::broadcastToSubscribers(_target, std::move(p));

    if (targetIsDead)
        World::broadcastAround(_target, Network::Packet::Server::ActorDiePacket(_target), true);

    // TODO: consume the soulshot charge here (not before because could have been canceled with stun/para/…)

    // Physical attacking never stops unless another action is requested (e.g. actor moves) or target dies
    if (!actor.nextAction() && !targetIsDead)
        actor.doNext<AttackAction>(_target, actor.stats().pAtkSpeed);
}
