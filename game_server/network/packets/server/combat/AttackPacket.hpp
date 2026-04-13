/// @author    Chnossos
/// @date      Created on 2026-03-21

#pragma once

// Project includes
#include "../../../../game/combat/Hit.hpp"

#include <l2cpp/network/Packet.hpp>

class Actor;

namespace Network::Packet::Server { class AttackPacket; }

class Network::Packet::Server::AttackPacket final : public l2cpp::Network::Packet
{
public:
    explicit AttackPacket(Actor const & attacker, Actor const & mainTarget);

public:
    void addHit(Hit const & hit);

private:
    Actor const & _attacker;
    Actor const & _mainTarget;
    size_t        _hitsCountOffset;
};
