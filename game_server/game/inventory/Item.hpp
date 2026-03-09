/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "ItemTemplate.hpp"
#include "../GameObject.hpp"

#include <l2cpp/network/Serialization.hpp>

namespace l2
{
    struct Item : public GameObject
    {
        DECLARE_PACKET_SERIALIZATION_OPERATOR(Item);

        ItemTemplate tmplate;

        u32  quantity     = 1;
        u8   enchantLevel = 0;
        bool equipped     = false;
    };
}
