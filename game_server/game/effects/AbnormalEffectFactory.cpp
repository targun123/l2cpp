/// @author    Chnossos
/// @date      Created on 2026-04-16

#include "AbnormalEffectFactory.hpp"

// Project includes
#include "../actor/Actor.hpp"
#include "../skill/SkillTemplate.hpp"

void DamageEffectFactory::apply(Actor const &, Actor & target)
{
    if (_elementType == DamageElementType::Poison)
        target.addAbnormalEffect<DamageEffect>(target, _skillTemplate.uid(), _elementType, 15, 10s, 1s);
    else
        target.addAbnormalEffect<DamageEffect>(target, _skillTemplate.uid(), _elementType, 250);
}

void BuffEffectFactory::apply(Actor const &, Actor & target)
{
    if (target.type() == ActorType::Character)
    {
        target.addAbnormalEffect<BuffEffect>(target, _skillTemplate.uid(),
            _skillTemplate.type() == SkillType::Toggle ? -1s : 20s, _modifiedStat, _value);
    }
}
