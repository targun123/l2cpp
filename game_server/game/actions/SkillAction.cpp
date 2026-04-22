/// @author    Chnossos
/// @date      Created on 2026-04-08

#include "SkillAction.hpp"

// Project includes
#include "../../network/packets/server/skill/SkillCancelPacket.hpp"
#include "../../network/packets/server/skill/SkillSetTargetsPacket.hpp"
#include "../../network/packets/server/skill/SkillUsePacket.hpp"
#include "../../network/packets/server/ui/UiGaugePacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../../utils/Target.hpp"
#include "../World.hpp"
#include "../components/Stats.hpp"
#include "../skill/Skill.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/utils/Enum.hpp>

namespace SC = Network::Packet::Server;

struct SkillAction::SkillActionImpl
{
    SkillTemplate const &   skill;
    bool                    forceAttack;
    ClockDuration           castingElapsed       = ClockDuration::zero();
    ClockDuration           castingDuration      = ClockDuration::zero();
    ClockDuration           notifyTargetsTrigger = ClockDuration::zero();
    std::vector<Ref<Actor>> targets;
};

template class Pimpl<SkillAction::SkillActionImpl>;

// ---------------------------------------------------------------------------------------------------------------------

SkillAction::SkillAction(Actor & performer, SkillTemplate const & skill, bool const forceAttack)
    : Action(ActionType::Skill, performer)
    , _impl(skill, forceAttack)
{
    L2CPP_B_ASSERT(l2cpp::Utils::Enum::isAnyOf(_impl->skill.type(), SkillType::Active, SkillType::Toggle),
                   "Unsupported skill type '{}'", std::to_underlying(_impl->skill.type()));

    if (_impl->skill.castDuration() != ClockDuration::zero())
    {
        if (_impl->skill.isMagic())
        {
            auto const mAtkSpeedRatio   = performer.stats()[StatId::MAtkSpeed] / 333;
            _impl->castingDuration      = Utils::Chrono::Clock::toDuration(_impl->skill.castDuration() / mAtkSpeedRatio);

            if (_impl->castingDuration <= 550ms) // Avoid broken animations
                _impl->castingDuration  = 550ms;
        }
        else
        {
            auto const pAtkSpeedRatio   = performer.stats()[StatId::PAtkSpeed] / 300;
            _impl->castingDuration      = Utils::Chrono::Clock::toDuration(_impl->skill.castDuration() / pAtkSpeedRatio);

            if (_impl->castingDuration < 500ms) // Avoid broken animations
                _impl->castingDuration = 500ms;
        }

        // Skill targets must be notified 350ms (including latency) before the end of the animation to display correctly
        _impl->notifyTargetsTrigger = _impl->castingDuration - 350ms;
    }

    if (_impl->skill.targetType() == SkillTargetType::Self)
    {
        _impl->targets.emplace_back(performer);
    }
    else if (_impl->skill.targetType() != SkillTargetType::Aura)
    {
        if (auto const target = performer.target())
            _impl->targets.emplace_back(target);
        else
            L2CPP_THROW("Skill '{}' needs a target to be performed", _impl->skill.fullName());
    }
}

SkillAction::SkillAction(SkillAction &&) noexcept = default;
SkillAction & SkillAction::operator=(SkillAction &&) noexcept = default;

bool SkillAction::canBeInterruptedByAnotherAction() const { return false; }

void SkillAction::onStarted()
{
    if (_impl->skill.type() == SkillType::Active)
    {
        World::send(performer(), SC::UiGaugePacket{GaugeColor::Blue, _impl->castingDuration});
        SC::SkillUsePacket p{performer(), _impl->targets.empty() ? performer() : _impl->targets.at(0).get(),
                             _impl->skill.uid(), _impl->castingDuration, 1s, false};
        World::broadcastAround(performer(), std::move(p), true);
    }
    else // Toggle
        setFinished(true);
}

void SkillAction::updateImpl(ClockDuration const elapsed)
{
    // TODO: ensure target is still valid

    using enum SkillTargetType;
    if (l2cpp::Utils::Enum::isAnyOf(_impl->skill.targetType(), AoE, Aura) &&
        Utils::Chrono::thresholdCrossed(_impl->castingElapsed, elapsed, _impl->notifyTargetsTrigger))
    {
        selectTargets();

        std::vector<Ref<Actor const>> targets;
        targets.assign_range(_impl->targets);

        World::broadcastAround(performer(),
                               SC::SkillSetTargetsPacket{performer(), _impl->skill, targets}, true);
    }

    setFinished((_impl->castingElapsed += elapsed) >= _impl->castingDuration);
}

void SkillAction::onFinished()
{
    L2CPP_B_ASSERT(!_impl->targets.empty(), "No target at the end of skill casting, cannot apply effects");

    for (auto & target : _impl->targets)
        _impl->skill.applyEffects(performer(), target);
}

void SkillAction::onCanceled()
{
    World::broadcastAround(performer(), SC::SkillCancelPacket{performer()}, true);
}

void SkillAction::selectTargets()
{
    L2CPP_B_ASSERT(_impl->skill.type() == SkillType::Active,
                   "Skill whose type is not Active inside {}", __FUNCTION__);

    auto const & target = _impl->targets.empty() ? performer() : _impl->targets.at(0).get();

    World::forEachActorAround(target, [&] (auto & a)
    {
        if (Utils::Target::isValidTarget(performer(), _impl->skill, a, _impl->forceAttack))
            _impl->targets.emplace_back(a);
    });
}
