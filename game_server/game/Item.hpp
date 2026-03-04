/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "ItemTemplate.hpp"

namespace l2
{
    struct Item
    {
        ItemTemplate tmplate;

        u32  uid          = 0;
        u32  quantity     = 1;
        u16  enchantLevel = 0;
    };
}
