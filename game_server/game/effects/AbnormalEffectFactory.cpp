/// @author    Chnossos
/// @date      Created on 2026-04-16

#include "AbnormalEffectFactory.hpp"

// Project includes
#include "../actor/Actor.hpp"
#include "../skill/SkillTemplate.hpp"

AbnormalEffectFactory::AbnormalEffectFactory(
    AbnormalEffectType const   type
  , SkillTemplate      const & skillTemplate
  , ClockDuration      const   totalDuration
  , ClockDuration      const   tickDuration
  , ClockDuration      const   initialTriggerDuration
)
    : _skillTemplate(skillTemplate)
    , _totalDuration(totalDuration)
    , _tickDuration(tickDuration)
    , _initialTriggerDuration(initialTriggerDuration)
    , _type(type)
{}

void DamageEffectFactory::apply(Actor &, Actor & target)
{
    target.addAbnormalEffect<DamageEffect>(target, _skillTemplate.uid(), _elementType, _power,
                                           _totalDuration, _tickDuration, _initialTriggerDuration);
}

void HealEffectFactory::apply(Actor &, Actor & target)
{
    target.addAbnormalEffect<HealEffect>(target, _skillTemplate.uid(), _power,
                                         _totalDuration, _tickDuration, _initialTriggerDuration);
}

void BuffEffectFactory::apply(Actor & source, Actor & target)
{
    if (target.type() == source.type()) // characters can buff other characters, monsters can buff other monsters
    {
        target.addAbnormalEffect<BuffEffect>(target, _skillTemplate.uid(),
            _skillTemplate.type() == SkillType::Toggle ? -1s : _totalDuration, _modifiedStat, _value);
    }
}
