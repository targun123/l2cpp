/// @author    Chnossos
/// @date      Created on 2026-02-28

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace l2 { class Character; }

namespace Network::Packet::Server { class CharacterStatusUpdatePacket; }

class Network::Packet::Server::CharacterStatusUpdatePacket final : public l2cpp::Network::Packet
{
public:
    explicit CharacterStatusUpdatePacket(l2::Character &);
};
