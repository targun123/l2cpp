/// @author    Chnossos
/// @date      Created on 2026-05-14

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class ShortcutBar;

namespace Network::Packet::Server { struct UiShortcutListPacket; }

struct Network::Packet::Server::UiShortcutListPacket final : public l2cpp::Network::Packet
{
    explicit UiShortcutListPacket(ShortcutBar const &);
};
