/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;
class Character;

namespace Network::Packet::Server { struct TargetSelectPacket; }

struct Network::Packet::Server::TargetSelectPacket final : public l2cpp::Network::Packet
{
    explicit TargetSelectPacket(Character const & source, Actor const & target);
};
