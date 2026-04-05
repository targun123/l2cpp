/// @author    Chnossos
/// @date      Created on 2026-04-05

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Character;

namespace Network::Packet::Server { struct TargetClearPacket; }

struct Network::Packet::Server::TargetClearPacket final : public l2cpp::Network::Packet
{
    explicit TargetClearPacket(Character const &);
};
