/// @author    Chnossos
/// @date      Created on 2026-03-03

// Project includes
#include "../game/actor/Character.hpp"
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(UiShortcutUnset)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 index;
    reader >> index;
    L2CPP_B_ASSERT(index < Constants::maxShortcuts,
                   "Trying to remove a shortcut outside the valid index range, at index {}", index);

    player.currentCharacter()->delShortcut(index);
}
