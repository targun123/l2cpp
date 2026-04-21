/// @author    Chnossos
/// @date      Created on 2026-04-08

#include "SkillAction.hpp"

// Project includes
#include "../../network/packets/server/skill/SkillCancelPacket.hpp"
#include "../../network/packets/server/skill/SkillSetTargetsPacket.hpp"
#include "../../network/packets/server/skill/SkillUsePacket.hpp"
#include "../../network/packets/server/ui/UiGaugePacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../World.hpp"
#include "../components/Stats.hpp"
#include "../skill/Skill.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/utils/Enum.hpp>

namespace SC = Network::Packet::Server;

struct SkillAction::SkillActionImpl
{
    Skill &                 skill;
    ClockDuration           castingElapsed    = ClockDuration::zero();
    ClockDuration           castingDuration   = ClockDuration::zero();
    ClockDuration           ninetyPercentCast = ClockDuration::zero();
    std::vector<Ref<Actor>> targets;
};

template class Pimpl<SkillAction::SkillActionImpl>;

// ---------------------------------------------------------------------------------------------------------------------

SkillAction::SkillAction(Actor & performer, Skill & skill)
    : Action(ActionType::Skill, performer)
    , _impl(skill)
{
    auto const & tmpl = skill.tmplate();

    L2CPP_B_ASSERT(l2cpp::Utils::Enum::isAnyOf(tmpl.type(), SkillType::Active, SkillType::Toggle),
                   "Unsupported skill type '{}'", std::to_underlying(tmpl.type()));

    if (tmpl.castDuration() != ClockDuration::zero())
    {
        if (tmpl.isMagic())
        {
            auto const mAtkSpeedRatio = performer.stats()[StatId::MAtkSpeed] / 333;
            _impl->castingDuration    = Utils::Chrono::Clock::toDuration(tmpl.castDuration() / mAtkSpeedRatio);

            if (_impl->castingDuration <= 550ms)
                _impl->castingDuration  = 550ms;
        }
        else
        {
            auto const pAtkSpeedRatio = performer.stats()[StatId::PAtkSpeed] / 300;
            _impl->castingDuration    = Utils::Chrono::Clock::toDuration(tmpl.castDuration() / pAtkSpeedRatio);

            if (_impl->castingDuration < 500ms)
                _impl->castingDuration = 500ms;
        }
    }

    _impl->ninetyPercentCast = Utils::Chrono::Clock::toDuration(_impl->castingDuration * 0.9);

    if (tmpl.targetType() == SkillTargetType::None)
        _impl->targets.emplace_back(performer);
    else if (auto const target = performer.target())
        _impl->targets.emplace_back(target);
    else
        L2CPP_THROW("Skill '{}' needs a target to be performed", static_cast<u32>(skill.tmplate().uid()));
}

SkillAction::SkillAction(SkillAction &&) noexcept = default;
SkillAction & SkillAction::operator=(SkillAction &&) noexcept = default;

bool SkillAction::canBeInterruptedByAnotherAction() const { return false; }

void SkillAction::onStarted()
{
    if (_impl->skill.tmplate().type() == SkillType::Active)
    {
        World::send(performer(), SC::UiGaugePacket{GaugeColor::Blue, _impl->castingDuration});
        SC::SkillUsePacket p{performer(), _impl->skill.tmplate().uid(), _impl->castingDuration, /*FIXME*/1s, false};
        World::broadcastAround(performer(), std::move(p), true);
    }
    else // Toggle
        setFinished(true);
}

void SkillAction::updateImpl(ClockDuration const elapsed)
{
    // TODO: ensure target is still valid

    if (Utils::Chrono::thresholdCrossed(_impl->castingElapsed, elapsed, _impl->ninetyPercentCast))
    {
        selectTargets();

        std::vector<Ref<Actor const>> targets;
        targets.assign_range(_impl->targets);

        World::broadcastAround(_impl->targets.at(0),
                               SC::SkillSetTargetsPacket{performer(), _impl->skill, targets}, true);
    }

    setFinished((_impl->castingElapsed += elapsed) >= _impl->castingDuration);
}

void SkillAction::onFinished()
{
    L2CPP_B_ASSERT(!_impl->targets.empty(), "No target at the end of skill casting, cannot apply effects");

    for (auto & target : _impl->targets)
        _impl->skill.tmplate().applyEffects(performer(), target);
}

void SkillAction::onCanceled()
{
    World::broadcastAround(performer(), SC::SkillCancelPacket{performer()}, true);
}

void SkillAction::selectTargets()
{
    L2CPP_B_ASSERT(_impl->skill.tmplate().type() == SkillType::Active,
                   "Skill whose type is not Active inside {}", __FUNCTION__);

    switch (_impl->skill.tmplate().targetType())
    {
        case SkillTargetType::AoE:
            World::forEachActorAround(_impl->targets.at(0), [&] (auto & a) { _impl->targets.emplace_back(a); });
            break;

        default:
            break;
    }
}
