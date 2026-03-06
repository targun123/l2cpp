/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(TargetClear)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot clear target, no current character");

    auto & c = player.currentCharacter()->get();
    c.targetId.reset();
    player.connection().send(Packet(0x2a) << c.id() << c.pos.x << c.pos.y << c.pos.z);
}
