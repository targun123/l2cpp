/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(Move)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    i32 targetX, targetY, targetZ;
    i32 originX, originY, originZ;
    i32 movement;

    reader >> targetX >> targetY >> targetZ;
    reader >> originX >> originY >> originZ;
    reader >> movement;

    auto & c = player.characters().front();
    c.pos.x = targetX;
    c.pos.y = targetY;
    c.pos.z = targetZ;

    Packet p(0x01);
    p << c.id
      << targetX
      << targetY
      << targetZ
      << originX
      << originY
      << originZ
    ;
    player.connection().send(p);
}
