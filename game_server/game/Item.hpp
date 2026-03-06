/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "GameObject.hpp"
#include "ItemTemplate.hpp"

#include <l2cpp/network/Packet.hpp>

namespace l2
{
    struct Item : public GameObject
    {
        ItemTemplate tmplate;

        u32  quantity    = 1;
        s8  enchantLevel = 0;
        bool equipped    = false;
    };
}

l2cpp::Network::Packet & operator<<(l2cpp::Network::Packet & p, l2::Item const & item);
