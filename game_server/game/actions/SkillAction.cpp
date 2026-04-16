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

// C++ includes
#include <mutex>

namespace SC = Network::Packet::Server;

struct SkillAction::SkillActionImpl
{
    Skill &        skill;
    ClockDuration  castingElapsed = ClockDuration::zero();
    ClockDuration  ninetyPercenCast;
    std::once_flag targetsSentFlag;
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
}

void SkillAction::updateImpl(ClockDuration const elapsed)
{
    _impl->castingElapsed += elapsed;

    if (_impl->castingElapsed >= _impl->ninetyPercenCast) std::call_once(_impl->targetsSentFlag, [&]
    {
        Actor const & a = performer();

        _impl->targets.emplace_back(a.target());
        World::forEachActorAround(a.target(), [&] (Actor const & actor) { _impl->targets.emplace_back(actor); });
        World::broadcastAround(a, SC::SkillSetTargetsPacket{a, _impl->skill, _impl->targets}, true);
    });

    setFinished(_impl->castingElapsed >= _impl->skill.tmplate().castDuration());
}

void SkillAction::onFinished()
{
    // skill animation done, apply effects now
    if (auto const target = performer().target())
    {
        for (auto const & effect : _impl->skill.tmplate().effects())
        {
            effect->make(performer(), target);
        }
    }
}

void SkillAction::onCancelled()
{
    World::broadcastAround(performer(), SC::SkillCancelPacket{performer()}, true);
}
