/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(MoveUpdate)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    auto & c = player.currentCharacter()->get();

    s32 x, y;
    reader >> x >> y >> c.pos.z >> c.headAngle;

    auto & move = static_cast<MoveAction &>(*player.actions().front());

    auto const now             = std::chrono::steady_clock::now();
    auto const elapsed         = std::chrono::duration_cast<std::chrono::milliseconds>(now - move.lastUpdateTime);
    auto const unitSpeed       = c.finalStats.runSpeed / 1000.f;
    auto const distanceCovered = elapsed.count() * unitSpeed;

    move.lastUpdateTime = now;
    if ((move.currentDistance += distanceCovered) >= move.totalDistance) // Reached destination
    {
        Packet p(0x61);
        p << c.id << x << y << c.pos.z << c.headAngle;
        player.connection().send(p);
    }
}
