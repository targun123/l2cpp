/// @author    Chnossos
/// @date      Created on 2026-04-24

#include "ResurrectionEffect.hpp"

// Project includes
#include "../actor/Actor.hpp"

ResurrectionEffect::ResurrectionEffect(Actor & source, Actor & target, SkillUid const skillUid)
    : AbnormalEffect(AbnormalEffectType::Resurrection, source, target, skillUid)
{}

void ResurrectionEffect::onStarted()
{
    target().resurrect();
}
