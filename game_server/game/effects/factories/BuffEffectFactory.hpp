/// @author    Chnossos
/// @date      Created on 2026-04-24

#pragma once

// Project includes
#include "../../constants/StatId.hpp"
#include "AbnormalEffectFactory.hpp"

class BuffEffectFactory : public AbnormalEffectFactory
{
public:
    BuffEffectFactory(SkillTemplate const & skillTemplate, ClockDuration duration, StatId modifiedStat, double value);

public:
    void apply(Actor & source, Actor & target) override;

private:
    StatId _modifiedStat;
    double _value;
};

