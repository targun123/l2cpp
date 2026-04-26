/// @author    Chnossos
/// @date      Created on 2026-04-24

#pragma once

// Project includes
#include "AbnormalEffectFactory.hpp"

class ResurrectionEffectFactory : public AbnormalEffectFactory
{
public:
    explicit ResurrectionEffectFactory(SkillTemplate const & skillTemplate);

public:
    void apply(Actor & source, Actor & target) override;
};
