/// @author    Chnossos
/// @date      Created on 2026-04-24

#pragma once

// Project includes
#include "../constants/DamageElementType.hpp"
#include "AbnormalEffect.hpp"

class DamageEffect : public AbnormalEffect
{
public:
    DamageEffect(Actor &           target,
                 SkillUid          skillUid,
                 DamageElementType type,
                 StatValue         damage,
                 ClockDuration     effectDuration         = ClockDuration::zero(),
                 ClockDuration     tickDuration           = ClockDuration::zero(),
                 ClockDuration     initialTriggerDuration = ClockDuration::zero());

private:
    void onTick() override;

private:
    DamageElementType _elementType;
    StatValue         _damage;
};

