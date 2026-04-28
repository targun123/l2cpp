/// @author    Chnossos
/// @date      Created on 2026-03-02

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/ui/Shortcut.hpp"

DEFINE_PACKET_HANDLER(ShortcutBarAdd)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    Shortcut s;
    reader >> s;

    auto const & slot = player.currentCharacter()->setShortcut(std::move(s));
    player.connection().send(Packet(0x44) << slot);
}
