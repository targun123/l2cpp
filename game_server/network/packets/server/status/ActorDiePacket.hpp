/// @author    Chnossos
/// @date      Created on 2026-04-03

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct ActorDiePacket; }

struct Network::Packet::Server::ActorDiePacket final : public l2cpp::Network::Packet
{
    explicit ActorDiePacket(Actor const &);
};
