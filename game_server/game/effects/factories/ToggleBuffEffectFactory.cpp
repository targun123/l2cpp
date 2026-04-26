/// @author    Chnossos
/// @date      Created on 2026-04-24

#include "ToggleBuffEffectFactory.hpp"

ToggleBuffEffectFactory::ToggleBuffEffectFactory(SkillTemplate const & skillTemplate,
                                                 StatId const modifiedStat, double const value)
        : BuffEffectFactory(skillTemplate, -1s, modifiedStat, value)
{}

