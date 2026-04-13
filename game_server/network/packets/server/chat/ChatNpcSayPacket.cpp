/// @author    Chnossos
/// @date      Created on 2026-03-21

#include "ChatNpcSayPacket.hpp"

// Project includes
#include "../../../../game/actor/NonPlayableActor.hpp"
#include "../../../../game/components/NpcAppearance.hpp"

using Network::Packet::Server::ChatNpcSayPacket;

ChatNpcSayPacket::ChatNpcSayPacket(Actor const & emitter, ChatType type, std::wstring_view const & message)
    : Packet(0x02, "ChatNpcSay")
{
    L2CPP_B_ASSERT(emitter.type() != ActorType::Character,
                   "Packet ChatNpcSayPacket is incompatible with Character emitter");

    auto & nonPlayableActor = static_cast<NonPlayableActor const &>(emitter);

    *this
        << emitter.id()
        << type
        << nonPlayableActor.appearance().id()
        << message
    ;
}
