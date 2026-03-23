/// @author    Chnossos
/// @date      Created on 2026-03-21

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct AttackStanceTogglePacket; }

struct Network::Packet::Server::AttackStanceTogglePacket final : public l2cpp::Network::Packet
{
    AttackStanceTogglePacket(bool enable, Actor const & target);
};
