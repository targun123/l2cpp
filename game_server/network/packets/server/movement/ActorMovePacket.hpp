/// @author    Chnossos
/// @date      Created on 2026-04-09

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;
struct Position;

namespace Network::Packet::Server { struct ActorMovePacket; }

struct Network::Packet::Server::ActorMovePacket final : public l2cpp::Network::Packet
{
    ActorMovePacket(Actor const & actor, Position const & origin, Position const & destination);
};
