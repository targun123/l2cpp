/// @author    Chnossos
/// @date      Created on 2026-04-08

#include "SkillUsePacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"
#include "../../../../game/components/Position.hpp"
#include "../../../../game/skill/Skill.hpp"

using Network::Packet::Server::SkillUsePacket;

SkillUsePacket::SkillUsePacket(Actor const & caster, Skill const & skill, bool const isCritical)
    : Packet(0x48)
{
    // This is not the skill target, only the target for the skill animation during casting
    Actor const & target = caster.target() ? caster.target() : caster;

    *this
        << caster.id()
        << target.id()
        << static_cast<u32>(skill.tmplate().id())
        << static_cast<u32>(skill.tmplate().level())
        << skill.tmplate().castDuration()
        << 1000 // reuse delay (in ms)
        << caster.position()
        << (isCritical ? 1 : 0)
    ;
}
