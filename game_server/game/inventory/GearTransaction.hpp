/// @author    Chnossos
/// @date      Created on 2026-03-09

#pragma once

// Project includes
#include "../inventory/Item.hpp"

#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <unordered_map>

struct GearTransaction
{
    bool succeeded = true;               ///< Whether equip/unequip action has worked
    OptionalRef<Item const> target;  ///< Item that the user wants to equip/unequip
    OptionalRef<Item const> curItem; ///< Item that has been successfully equipped or nullopt if unequip successful

    /// Items that were in the slots prior to the transaction
    std::unordered_map<GameObjectId, std::reference_wrapper<Item const>> oldItems;
};
