/// @author    Chnossos
/// @date      Created on 2026-04-28

#pragma once

// Project includes
#include "../../../../game/constants/SocialAction.hpp"

#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct SocialActionPerformPacket; }

struct Network::Packet::Server::SocialActionPerformPacket final : public l2cpp::Network::Packet
{
    SocialActionPerformPacket(Actor const & performer, SocialAction actionId);
};
