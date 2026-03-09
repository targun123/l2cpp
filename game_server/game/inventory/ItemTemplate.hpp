/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include <string>

#include "GearSlot.hpp"
#include "ItemCategory.hpp"

struct ItemTemplate
{
    u32          id = 0;
    std::string  name;
    ItemCategory category = ItemCategory::Unknown;
    GearSlot     bodyPart = GearSlot::None;
};
