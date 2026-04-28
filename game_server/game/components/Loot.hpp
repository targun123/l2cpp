/// @author    Chnossos
/// @date      Created on 2026-04-27

#pragma once

// C++ includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

struct Loot : public Component
{
    u32 xp = 0;
    u32 sp = 0;
};
