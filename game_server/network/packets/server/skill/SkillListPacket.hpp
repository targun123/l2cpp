/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

// Project includes
#include "../../../../game/components/SkillDirectory.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct SkillListPacket; }

struct Network::Packet::Server::SkillListPacket final : public l2cpp::Network::Packet
{
    explicit SkillListPacket(SkillDirectory const &);
};
