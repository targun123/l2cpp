/// @author    Chnossos
/// @date      Created on 2026-03-02

// Project includes
#include "../game/actor/Character.hpp"
#include "../game/ui/Shortcut.hpp"
#include "../network/packets/server/ui/UiShortcutSetPacket.hpp"
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(UiShortcutSet)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    Shortcut s;
    reader >> s;

    player.connection().send(UiShortcutSetPacket{player.currentCharacter()->setShortcut(std::move(s))});
}
