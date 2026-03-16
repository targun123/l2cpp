/// @author    Chnossos
/// @date      Created on 2026-03-15

#pragma once

// Project includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

struct Position : public Component
{
    s32 x, y, z;

    explicit Position(s32 const x_ = 0, s32 const y_ = 0, s32 const z_ = 0) noexcept
        : x(x_), y(y_), z(z_)
    {}
};
