/// @author    Chnossos
/// @date      Created on 2026-04-08

#pragma once

// Project includes
#include "../../../../game/skill/SkillUid.hpp"

#include <l2cpp/network/Packet.hpp>

class Actor;
class Skill;

namespace Network::Packet::Server { struct SkillUsePacket; }

struct Network::Packet::Server::SkillUsePacket final : public l2cpp::Network::Packet
{
    SkillUsePacket(Actor const & caster, SkillUid skillUid,
                   ClockDuration castDuration, ClockDuration cooldown, bool isCritical);
};
