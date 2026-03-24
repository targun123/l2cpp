/// @author    Chnossos
/// @date      Created on 2026-03-03

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/Shortcut.hpp"

DEFINE_PACKET_HANDLER(ShortcutBarRemove)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 index;
    reader >> index;
    L2CPP_B_ASSERT(index < 120, "Trying to remove a shortcut outside the valid index range: {}", index);

    player.currentCharacter()->delShortcut(index);
}
