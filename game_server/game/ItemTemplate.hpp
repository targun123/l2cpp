/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "BodyPart.hpp"
#include "ItemCategory.hpp"

namespace l2
{
    struct ItemTemplate
    {
        u32          id       = 0;
        ItemCategory category = ItemCategory::Unknown;
        BodyPart     bodyPart = BodyPart::None;
    };
}
