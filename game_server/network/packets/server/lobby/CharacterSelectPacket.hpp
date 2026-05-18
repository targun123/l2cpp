/// @author    Chnossos
/// @date      Created on 2026-03-31

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Character;

namespace Network::Packet::Server { struct CharacterSelectPacket; }

struct Network::Packet::Server::CharacterSelectPacket final : public l2cpp::Network::Packet
{
    CharacterSelectPacket(Character const & c, u32 encryptionKey);
};
