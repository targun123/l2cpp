/// @author    Chnossos
/// @date      Created on 2026-05-14

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Shortcut;

namespace Network::Packet::Server { struct UiShortcutSetPacket; }

struct Network::Packet::Server::UiShortcutSetPacket final : public l2cpp::Network::Packet
{
    explicit UiShortcutSetPacket(Shortcut const &);
};
