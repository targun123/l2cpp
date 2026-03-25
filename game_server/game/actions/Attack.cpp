/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "Attack.hpp"

// Project includes
#include "../../Player.hpp"
#include "../../network/Connection.hpp"
#include "../../network/packets/server/chat/ChatNpcSayPacket.hpp"
#include "../../network/packets/server/combat/AttackPacket.hpp"
#include "../../network/packets/server/combat/AttackStanceTogglePacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../actor/Character.hpp"
#include "../components/Gear.hpp"

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

AttackAction::AttackAction(u32 const pAtkSpeed) noexcept
    : Action(Type::Attack)
    , hitDuration(toClockDuration(1s / (pAtkSpeed / 500.)))
    , impactTimePoint(toClockDuration(hitDuration * 0.55))
{}

bool AttackAction::canBeInterrupted() const
{
    return false;
}

void AttackAction::onStarted(Actor & actor)
{
    auto & c = static_cast<Character &>(actor);

    c.state = ActorState::Attacking;

    // Use soulshots if a weapon is equipped
    std::optional<ItemGrade> soulShotGrade;
    if (auto const weapon = c.gear().weapon(); weapon)
        soulShotGrade = weapon->tmplate.grade;

    c.player->connection().send(SM::AttackPacket(c, c.target(), {c.target(), 10, false, soulShotGrade}));
}

void AttackAction::onFinished(Actor & actor)
{
    // Physical attacks never finish unless another action was queued (e.g. player moves) or target dies / is canceled
    if (!actor.nextAction() && actor.target())
        restart();
    else
        actor.state = ActorState::CombatIdle;
}

void AttackAction::updateImpl(ClockDuration const elapsed, Actor & actor)
{
    auto const target = actor.target();
    if (!target)
        return setFinished(true);

    if (Utils::Chrono::thresholdCrossed(lastUpdateTime(), elapsed, impactTimePoint))
    {
        auto const & c = static_cast<Character &>(actor);

        // Enable attack stance on opponents
        c.player->connection().send(SM::AttackStanceTogglePacket(true, c));
        c.player->connection().send(SM::AttackStanceTogglePacket(true, target));

        // Make the target go Ouch!
        static bool toggle;
        c.player->connection().send(SM::ChatNpcSayPacket(target, ChatType::General, toggle ? L"Ouch!" : L"Waah!"));
        toggle = !toggle;
    }

    setFinished(lastUpdateTime() >= startTime() + hitDuration);
}
