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
#include "../skill/Skill.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/utils/Enum.hpp>

namespace SC = Network::Packet::Server;

struct SkillAction::SkillActionImpl
{
    Skill &                 skill;
    ClockDuration           castingElapsed = ClockDuration::zero();
    ClockDuration           ninetyPercentCast;
    std::vector<Ref<Actor>> targets;
};

template class Pimpl<SkillAction::SkillActionImpl>;

// ---------------------------------------------------------------------------------------------------------------------

SkillAction::SkillAction(Actor & performer, Skill & skill)
    : Action(ActionType::Skill, performer)
    , _impl(skill)
{
    L2CPP_B_ASSERT(l2cpp::Utils::Enum::isAnyOf(skill.tmplate().type(), SkillType::Active, SkillType::Toggle),
                   "Unsupported skill type '{}'", std::to_underlying(skill.tmplate().type()));

    _impl->ninetyPercentCast = Utils::Chrono::Clock::toDuration(skill.tmplate().castDuration() * 0.9);

    if (skill.tmplate().targetType() == SkillTargetType::None)
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
        World::send(performer(), SC::UiGaugePacket{GaugeColor::Blue, _impl->skill.tmplate().castDuration()});
        World::broadcastAround(performer(), SC::SkillUsePacket{performer(), _impl->skill, false}, true);
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

    setFinished((_impl->castingElapsed += elapsed) >= _impl->skill.tmplate().castDuration());
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
