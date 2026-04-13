/// @author    Chnossos
/// @date      Created on 2026-03-31

#include "ChatSystemSayPacket.hpp"

using Network::Packet::Server::ChatSystemSayPacket;

ChatSystemSayPacket::ChatSystemSayPacket(u32 const messageId)
    : Packet(0x64, "ChatSystemSay")
{
    *this << messageId;
}

ChatSystemSayPacket & ChatSystemSayPacket::appendArg(std::unique_ptr<details::SystemMessageArgument> arg)
{
    _args.emplace_back(std::move(arg));
    return *this;
}

void ChatSystemSayPacket::finalizeImpl()
{
    *this << static_cast<u32>(_args.size());

    for (auto const & arg : _args)
        arg->serialize(*this);
}
