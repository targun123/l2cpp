/// @author    Chnossos
/// @date      Created on 2026-03-06

#pragma once

// Project includes
#include "../../../../game/inventory/Item.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct InventoryUpdatePacket; }

struct Network::Packet::Server::InventoryUpdatePacket final : public l2cpp::Network::Packet
{
    InventoryUpdatePacket();

    void appendAddedItem   (Item const &);
    void appendModifiedItem(Item const &);
    void appendRemovedItem (Item const &);
};
