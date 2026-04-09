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

AttackAction::AttackAction(Actor & performer, Actor & target, u32 const pAtkSpeed) noexcept
    : Action(ActionType::Attack, performer)
    , _target(target)
    , _hitDuration(Utils::Chrono::Clock::toDuration(1s / (pAtkSpeed / 500.)))
{}

bool AttackAction::canBeInterruptedByAnotherAction() const { return false; }

void AttackAction::onStarted()
{
    performer().state = ActorState::Attacking;

    u8 hitCount = 1;

    std::optional<ItemGrade> soulShotGrade;
    if (performer().type() == ActorType::Character)
    {
        auto & c = static_cast<Character &>(performer());

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

    SM::AttackPacket p(performer(), _target);
    for (decltype(hitCount) i = 0; i < hitCount; ++i) // split dual hits damage
        p.addHit({_target, 50u / hitCount, false, soulShotGrade});

    World::broadcastAround(performer(), std::move(p), true);
}

void AttackAction::updateImpl(ClockDuration const)
{
    setFinished(lastUpdateTime() >= startTime() + _hitDuration);
}

void AttackAction::onFinished()
{
    auto & actor = performer();

    // Enable attack stance on target once it gets hit
    World::broadcastAround(_target, SM::AttackStanceTogglePacket(true, _target), true);

    actor  .getOrAddComponent<AttackStanceTimer>().restart();
    _target.getOrAddComponent<AttackStanceTimer>().restart();

    auto & stats = *_target.component<ComputedStats>();
    bool targetIsDead = stats.curHp > 0;
    if ((stats.curHp -= 500) <= 0)
    {
        stats.curHp = 0;
        targetIsDead = true;
    }

    Network::Packet::Server::StatsUpdatePacket p(_target);
    p.addStat(Stat::CurHp, static_cast<u32>(stats.curHp));
    World::broadcastToSubscribers(_target, std::move(p));

    if (targetIsDead)
    {
        World::broadcastAround(_target, Network::Packet::Server::ActorDiePacket(_target), true);

        if (_target.type() != ActorType::Character || !static_cast<Character &>(_target).player)
            World::scheduleForDeletion(_target, 5s); // Corpse will disappear soon
    }

    // TODO: consume the soulshot charge here (not before because could have been canceled with stun/para/…)

    // Physical attacking never stops unless another action is requested (e.g. actor moves) or target dies
    if (!actor.nextAction() && !targetIsDead)
        actor.doNext<AttackAction>(_target, actor.stats().pAtkSpeed);
}
