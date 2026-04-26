/// @author    Chnossos
/// @date      Created on 2026-04-24

#pragma once

// Project includes
#include "AbnormalEffect.hpp"

class ResurrectionEffect : public AbnormalEffect
{
public:
    ResurrectionEffect(Actor & source, Actor & target, SkillUid skillUid);

private:
    void onStarted() override;
};
