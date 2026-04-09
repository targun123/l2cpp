/// @author    Chnossos
/// @date      Created on 2026-04-09

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct ActorMoveStopPacket; }

struct Network::Packet::Server::ActorMoveStopPacket final : public l2cpp::Network::Packet
{
    explicit ActorMoveStopPacket(Actor const & actor);
};
