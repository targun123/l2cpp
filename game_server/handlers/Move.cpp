/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

// Taken from Ruk33
// ================
// float const distanceX = targetX - c.pos.x;
// float const distanceY = targetY - c.pos.y;
// float const distanceZ = targetZ - c.pos.z;
//
// float const distance = sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
// float const speed    = c.baseStats.runSpeed;
//
// float const ticksPerSecond = 10;
// u64   const ticksToMove    = ticksPerSecond * distance / speed;
//
// float const sin = distanceY / distance;
// float const cos = distanceX / distance;
//
// auto const xSpeedTicks = cos * speed / ticksPerSecond;
// auto const ySpeedTicks = sin * speed / ticksPerSecond;
//
// targetX = c.pos.x + static_cast<s32>(distance * cos);
// targetY = c.pos.y + static_cast<s32>(distance * sin);

DEFINE_PACKET_HANDLER(Move)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    MoveAction action;
    reader >> action.targetX >> action.targetY >> action.targetZ;
    reader >> action.originX >> action.originY >> action.originZ;
    reader >> action.input;

    // TODO: trust originX/Y/Z only if they are close enough to server position

    SPDLOG_TRACE("Moving requested to {{{}, {}, {}}} (input: {})",
                 action.targetX, action.targetY, action.targetZ,
                 (action.input == MoveAction::Input::Mouse ? "mouse" : "keyboard"));

    auto & c = player.currentCharacter()->get();
    float const distanceX = action.targetX - c.pos.x;
    float const distanceY = action.targetY - c.pos.y;
    float const distanceZ = action.targetZ - c.pos.z;
    action.totalDistance = std::sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);

    float const distanceCoveredInOneMs = c.finalStats.runSpeed / 1000.f;

    std::chrono::duration<float, std::milli> const totalDuration(action.totalDistance / distanceCoveredInOneMs);
    SPDLOG_TRACE("Character must travel through {} units. At a speed of {} units per 1s, it will take {:%Q%q}",
                 action.totalDistance, c.finalStats.runSpeed, std::chrono::duration_cast<std::chrono::seconds>(totalDuration));

    player.setNextAction<MoveAction>(action);

    Packet p(0x01); // Make character start moving, position will be validated in MoveUpdate handler
    p << c.id
      << action.targetX << action.targetY << action.targetZ
      << c.pos.x        << c.pos.y        << c.pos.z
    ;
    player.connection().send(p);
}
