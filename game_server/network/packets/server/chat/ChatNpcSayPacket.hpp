/// @author    Chnossos
/// @date      Created on 2026-03-21

#pragma once

// Project includes
#include "../../../../game/constants/ChatType.hpp"

#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { struct ChatNpcSayPacket; }

struct Network::Packet::Server::ChatNpcSayPacket final : public l2cpp::Network::Packet
{
    ChatNpcSayPacket(Actor const & emitter, ChatType type, std::wstring_view const & message);
};
