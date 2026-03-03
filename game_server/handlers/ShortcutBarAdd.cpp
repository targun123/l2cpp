/// @author    Chnossos
/// @date      Created on 2026-03-02

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../game/Shortcut.hpp"

DEFINE_PACKET_HANDLER(ShortcutBarAdd)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    l2::Shortcut s;
    reader >> s;

    auto const index = *s.index();
    SPDLOG_TRACE("Adding shortcut to page {} slot {}", index / 12 + 1, index % 12 + 1);

    auto & slot = player.currentCharacter()->get().setShortcut(std::move(s));
    player.connection().send(Packet(0x44) << slot);
}
