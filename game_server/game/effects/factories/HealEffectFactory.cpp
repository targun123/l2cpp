/// @author    Chnossos
/// @date      Created on 2026-04-24

#include "HealEffectFactory.hpp"

// Project includes
#include "../../actor/Actor.hpp"
#include "../../skill/SkillTemplate.hpp"
#include "../HealEffect.hpp"

HealEffectFactory::HealEffectFactory(
    SkillTemplate const & skillTemplate
  , u32           const   power
  , ClockDuration const   totalDuration
  , ClockDuration const   tickDuration
  , ClockDuration const   initialTriggerDuration
)
    : AbnormalEffectFactory(AbnormalEffectType::Damage, skillTemplate,
                            totalDuration, tickDuration, initialTriggerDuration)
    , _power(power)
{}

void HealEffectFactory::apply(Actor &, Actor & target)
{
    target.addAbnormalEffect<HealEffect>(target, _skillTemplate.uid(), _power,
                                         _totalDuration, _tickDuration, _initialTriggerDuration);
}
