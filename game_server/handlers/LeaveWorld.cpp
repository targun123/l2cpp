/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(LeaveWorld)
{
    if (auto const c = player.currentCharacter())
    {
        World::moveCharacterBackToPreviews(c);
        player.unsetCurrentCharacter();
        player.connection().send(Packet(0x5f) << 1);
    }

    handleCharacterList(player);
}
