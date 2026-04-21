/// @author    Chnossos
/// @date      Created on 2026-03-31

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct ClientExitGamePacket; }

struct Network::Packet::Server::ClientExitGamePacket final : public l2cpp::Network::Packet
{
    ClientExitGamePacket();
};
