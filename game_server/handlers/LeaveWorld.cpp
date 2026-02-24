/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(LeaveWorld)
{
    player.connection().send(Packet(0x5f) << 1);
    handleCharacterList(player);
}
