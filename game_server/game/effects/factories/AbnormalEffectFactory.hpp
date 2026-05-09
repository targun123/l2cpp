/// @author    Chnossos
/// @date      Created on 2026-04-16

#pragma once

// Project includes
#include "../../../Typedefs.hpp"
#include "../../constants/AbnormalEffectType.hpp"

class Actor;
class SkillTemplate;

class AbnormalEffectFactory
{
public:
    AbnormalEffectFactory(
        AbnormalEffectType         type
      , SkillTemplate      const & skillTemplate
      , ClockDuration              totalDuration          = ClockDuration::zero()
      , ClockDuration              tickDuration           = ClockDuration::zero()
      , ClockDuration              initialTriggerDuration = ClockDuration::zero()
    );
    virtual ~AbnormalEffectFactory() = default;

public:
    auto type() const -> AbnormalEffectType { return _type; }

public:
    virtual void apply(Actor & source, Actor & target) = 0;

protected:
    SkillTemplate const & _skillTemplate;
    ClockDuration         _totalDuration;
    ClockDuration         _tickDuration;
    ClockDuration         _initialTriggerDuration;

private:
    AbnormalEffectType _type;
};
