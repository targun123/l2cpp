/// @author    Chnossos
/// @date      Created on 2026-04-16

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct AbnormalEffectListPacket; }

struct Network::Packet::Server::AbnormalEffectListPacket final : public l2cpp::Network::Packet
{
    explicit AbnormalEffectListPacket(Actor const &);
};
