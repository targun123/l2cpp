/// @author    Chnossos
/// @date      Created on 2026-05-14

#pragma once

// Project includes
#include "../../../../game/constants/Ui.hpp"

#include <l2cpp/network/Packet.hpp>

class Shortcut;

namespace Network::Packet::Server { struct UiShortcutListPacket; }

struct Network::Packet::Server::UiShortcutListPacket final : public l2cpp::Network::Packet
{
    explicit UiShortcutListPacket(std::span<Shortcut const, Constants::maxShortcuts>);
};
