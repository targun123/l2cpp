/// @author    Chnossos
/// @date      Created on 2026-03-06

#pragma once

// Project includes
#include "../../../../game/inventory/Item.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct InventoryListPacket; }

struct Network::Packet::Server::InventoryListPacket final : public l2cpp::Network::Packet
{
    InventoryListPacket(bool openInventory, std::span<std::reference_wrapper<l2::Item const> const> inventory);
};
