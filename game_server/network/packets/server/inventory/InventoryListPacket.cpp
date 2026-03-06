/// @author    Chnossos
/// @date      Created on 2026-03-06

#include "InventoryListPacket.hpp"

using Network::Packet::Server::InventoryListPacket;

InventoryListPacket::InventoryListPacket(bool const openInventory,
                                         std::span<std::reference_wrapper<l2::Item const> const> const inventory)
    : Packet(0x1b)
{
    *this
        << static_cast<u16>(openInventory)
        << static_cast<u16>(inventory.size())
    ;

    for (auto const & itemRef : inventory)
        *this << itemRef.get();
}
