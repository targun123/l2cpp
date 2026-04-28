/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include "../ecs/Component.hpp"
#include "../gameplay/ExperienceTable.hpp"

class ActorStatus : public Component
{
public:
    auto level() const -> u32 { return _level; }
    void setLevel(u32 const level)
    {
        L2CPP_B_ASSERT(ExperienceTable::minLevel() <= level && level <= ExperienceTable::maxLevel(),
                       "Invalid attempt to set level to '{}' (should be between [ {} ; {} ])",
                       level, ExperienceTable::minLevel(), ExperienceTable::maxLevel());

        _level = level;
    }

private:
    u32 _level = 1;
};
