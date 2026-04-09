/// @author    Chnossos
/// @date      Created on 2026-04-08

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct SkillCancelPacket; }

struct Network::Packet::Server::SkillCancelPacket final : public l2cpp::Network::Packet
{
    explicit SkillCancelPacket(Actor const & caster);
};
