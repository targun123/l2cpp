/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../network/packets/server/lobby/CharacterSelectPacket.hpp"

DEFINE_PACKET_HANDLER(CharacterSelect)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 index;
    reader >> index;

    auto const characterPreviews = World::getCharacterPreviews(player.accountName());
    L2CPP_B_ASSERT(index < characterPreviews.size(),
                   "Invalid character selection index '{}': should be less than {}", index, characterPreviews.size());

    player.setCurrentCharacter(World::loadCharacterFromPreview(characterPreviews[index]));
    player.connection().send(CharacterSelectPacket(player.currentCharacter(), player.playOk1()));
}
