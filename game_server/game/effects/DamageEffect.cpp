/// @author    Chnossos
/// @date      Created on 2026-04-24

#include "DamageEffect.hpp"

// Project includes
#include "../actor/Actor.hpp"

DamageEffect::DamageEffect(
    Actor &                   target
    , SkillUid          const skillUid
    , DamageElementType const type
    , StatValue         const damage
    , ClockDuration     const effectDuration
    , ClockDuration     const tickDuration
    , ClockDuration     const initialTriggerDuration
)
    : AbnormalEffect(AbnormalEffectType::Damage, target, skillUid, effectDuration, tickDuration, initialTriggerDuration)
    , _elementType(type)
    , _damage(damage)
{}

void DamageEffect::onTick()
{
    target().takeDamage(_damage);
}

