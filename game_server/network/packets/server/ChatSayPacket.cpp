/// @author    Chnossos
/// @date      Created on 2026-03-04

#include "ChatSayPacket.hpp"

using Network::Packet::Server::ChatSayPacket;

ChatSayPacket::ChatSayPacket(u32 senderId, std::wstring_view sender, ChatType type, std::wstring_view message)
    : Packet(0x4a)
{
    *this
        << senderId
        << type
        << sender
        << message
    ;
}
