/// @author    Chnossos
/// @date      Created on 2026-04-28

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

class ExperienceTable
{
    ExperienceTable() noexcept = default;

public:
    static auto level(u32 experience) -> u32;
};
