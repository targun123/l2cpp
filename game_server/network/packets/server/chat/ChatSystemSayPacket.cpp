/// @author    Chnossos
/// @date      Created on 2026-03-31

#include "ChatSystemSayPacket.hpp"

using Network::Packet::Server::ChatSystemSayPacket;

ChatSystemSayPacket::ChatSystemSayPacket(u32 const messageId)
    : Packet(0x64, "ChatSystemSay")
{
    *this << messageId;

    appendCounterAndStoreOffset(_argsCountOffset);
}

ChatSystemSayPacket & ChatSystemSayPacket::appendArgImpl(SystemMessageArgument const & arg)
{
    counterAtOffset(_argsCountOffset) += 1;
    arg.serialize(*this);
    return *this;
}
