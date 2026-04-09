/// @author    Chnossos
/// @date      Created on 2026-04-08

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;
class Skill;

namespace Network::Packet::Server { struct SkillSetTargetsPacket; }

struct Network::Packet::Server::SkillSetTargetsPacket final : public l2cpp::Network::Packet
{
    SkillSetTargetsPacket(Actor const & caster, Skill const & skill, std::span<Ref<Actor const> const> targets);
};
