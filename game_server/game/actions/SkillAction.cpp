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
    Skill &        skill;
    ClockDuration  castingElapsed = ClockDuration::zero();
    ClockDuration  ninetyPercenCast;
    std::vector<Ref<Actor const>> targets;
};

template class Pimpl<SkillAction::SkillActionImpl>;

// ---------------------------------------------------------------------------------------------------------------------

SkillAction::SkillAction(Actor & performer, Skill & skill)
    : Action(ActionType::Skill, performer)
    , _impl(skill)
{
    _impl->ninetyPercenCast = Utils::Chrono::Clock::toDuration(skill.tmplate().castDuration() * 0.9);
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
    if (Utils::Chrono::thresholdCrossed(_impl->castingElapsed, elapsed, _impl->ninetyPercenCast))
    {
        Actor const & a = performer();

        if (auto const target = a.target())
            _impl->targets.emplace_back(target);

        using enum SkillTargetType;
        if (l2cpp::Utils::Enum::isAnyOf(_impl->skill.tmplate().targetType(), Area/*, ...*/))
            World::forEachActorAround(a, [&] (Actor const & actor) { _impl->targets.emplace_back(actor); });

        World::broadcastAround(a, SC::SkillSetTargetsPacket{a, _impl->skill, _impl->targets}, true);
    }
    _impl->castingElapsed += elapsed;

    // TODO: ensure target is still valid

    setFinished(_impl->castingElapsed >= _impl->skill.tmplate().castDuration());
}

void SkillAction::onFinished()
{
    // skill animation ended (or no animation), apply effects now

    auto & target = _impl->skill.tmplate().targetType() == SkillTargetType::Self
                  ? performer() : *performer().target();

    for (auto const & effect : _impl->skill.tmplate().effects())
        effect->apply(performer(), target);
}

void SkillAction::onCancelled()
{
    World::broadcastAround(performer(), SC::SkillCancelPacket{performer()}, true);
}
