/// @author    Chnossos
/// @date      Created on 2026-03-04


#pragma once

// Project includes
#include "../../../game/ChatType.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct ChatSayPacket; }

struct Network::Packet::Server::ChatSayPacket final : public l2cpp::Network::Packet
{
    ChatSayPacket(u32 senderId, std::wstring_view sender, ChatType type, std::wstring_view message);
};
