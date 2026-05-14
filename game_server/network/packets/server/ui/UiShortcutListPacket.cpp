/// @author    Chnossos
/// @date      Created on 2026-05-14

#include "UiShortcutListPacket.hpp"

// Project includes
#include "../../../../game/ui/Shortcut.hpp"

using Network::Packet::Server::UiShortcutListPacket;

UiShortcutListPacket::UiShortcutListPacket(std::span<Shortcut const, Constants::maxShortcuts> shortcuts)
    : Packet(0x45, "UiShortcutList")
{
    *this << static_cast<u32>(shortcuts.size()) ;

    for (auto const & s : shortcuts)
        *this << s;
}
