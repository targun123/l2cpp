/// @author    Chnossos
/// @date      Created on 2026-04-08

#include "SkillUsePacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"
#include "../../../../game/components/Position.hpp"
#include "../../../../game/skill/Skill.hpp"

using Network::Packet::Server::SkillUsePacket;

SkillUsePacket::SkillUsePacket(
    Actor           const & caster
    , SkillUid      const   skillUid
    , ClockDuration const   castDuration
    , ClockDuration const   cooldown
    , bool          const   isCritical
)
    : Packet(0x48, "SkillUse")
{
    // This is not the skill target, only the target for the skill animation during casting
    Actor const & target = caster.target() ? caster.target() : caster;

    *this
        << caster.id()
        << target.id()
        << skillUid
        << std::chrono::floor<std::chrono::milliseconds>(castDuration)
        << std::chrono::floor<std::chrono::milliseconds>(cooldown)
        << caster.position()
        << (isCritical ? 1 : 0)
    ;
}
