/// @author    Chnossos
/// @date      Created on 2026-04-08

#include "SkillSetTargetsPacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"
#include "../../../../game/skill/Skill.hpp"

using Network::Packet::Server::SkillSetTargetsPacket;

SkillSetTargetsPacket::SkillSetTargetsPacket(Actor const & caster, Skill const & skill,
                                             std::span<Ref<Actor const> const> const targets)
    : Packet(0x76, "SkillSetTargets")
{
    *this
        << caster.id()
        << static_cast<u32>(skill.tmplate().id())
        << static_cast<u32>(skill.tmplate().level())
        << static_cast<u32>(targets.size())
    ;

    for (Actor const & target : targets)
        *this << target.id();
}
