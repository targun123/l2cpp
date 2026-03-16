/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"

DEFINE_PACKET_HANDLER(TargetSelect)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot select target, no current character");

    u32 targetId;
    s32 originX, originY, originZ;
    u8 actionId; // 0=click 1=shift+click

    PacketReader p(player.connection().readBuffer().subspan(3));
    p >> targetId >> originX >> originY >> originZ >> actionId;

    player.currentCharacter()->get().targetId = targetId;
    player.connection().send(Packet(0xa6) << targetId << static_cast<u16>(0));
}
