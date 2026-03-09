/// @author    Chnossos
/// @date      Created on 2026-03-06

#pragma once

// Project includes
#include "../../../../game/inventory/ItemStorage.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct InventoryListPacket; }

struct Network::Packet::Server::InventoryListPacket final : public l2cpp::Network::Packet
{
    InventoryListPacket(bool openInventory, ItemStorage const & inventory);
};
