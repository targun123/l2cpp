/// @author    Chnossos
/// @date      Created on 2026-04-24

#pragma once

// Project includes
#include "AbnormalEffect.hpp"

class BuffEffect : public AbnormalEffect
{
public:
    BuffEffect(Actor & source, Actor & target, SkillUid skillUid,
               ClockDuration duration, StatId modifiedStat, StatValue value);

private:
    void onStarted() override;
    void onFinished() override;
    void modifyStat(StatValue newValue) const;

private:
    StatId    _modifiedStat;
    StatValue _value;
};

