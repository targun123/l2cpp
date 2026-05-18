/// @author    Chnossos
/// @date      Created on 2026-05-06

#pragma once

// Project includes
#include "../../../../game/constants/CharacterCreationResult.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct CharacterCreatePacket; }

struct Network::Packet::Server::CharacterCreatePacket final : public l2cpp::Network::Packet
{
    explicit CharacterCreatePacket(CharacterCreationResult result);
};
