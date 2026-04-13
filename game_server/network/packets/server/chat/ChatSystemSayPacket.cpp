/// @author    Chnossos
/// @date      Created on 2026-03-31

#include "ChatSystemSayPacket.hpp"

using Network::Packet::Server::ChatSystemSayPacket;

ChatSystemSayPacket::ChatSystemSayPacket(u32 const messageId)
    : Packet(0x64, "ChatSystemSay")
{
    *this << messageId;

    _argsCountOffset = appendSize(0);
}

ChatSystemSayPacket & ChatSystemSayPacket::appendArg(details::SystemMessageArgument const && arg)
{
    sizeAtOffset<u32>(_argsCountOffset) += 1;
    arg.serialize(*this);
    return *this;
}
