/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/SkillListPacket.hpp"

DEFINE_PACKET_HANDLER(SkillList)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "No character, can't send skill list");
    player.connection().send(SkillListPacket(player.currentCharacter()->get().skills()));
}
