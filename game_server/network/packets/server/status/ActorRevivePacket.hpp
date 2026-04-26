/// @author    Chnossos
/// @date      Created on 2026-04-24

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct ActorRevivePacket; }

struct Network::Packet::Server::ActorRevivePacket final : public l2cpp::Network::Packet
{
    explicit ActorRevivePacket(Actor const & target);
};
