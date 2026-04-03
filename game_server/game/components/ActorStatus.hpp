/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include "../Gauge.hpp"
#include "../ecs/Component.hpp"

class ActorStatus : public Component
{
public:
    auto level() const -> u32 { return _level; }
    void setLevel(u32 const level)
    {
        L2CPP_B_ASSERT(0 < level && level <= 80,
                       "Invalid attempt to set level to '{}' (should be between [ 1 ; 80 ])", level);

        _level = level;
    }

private:
    u32 _level = 1;
};
