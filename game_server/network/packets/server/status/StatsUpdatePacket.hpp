/// @author    Chnossos
/// @date      Created on 2026-04-02

#pragma once

// Project includes
#include <unordered_map>
#include <l2cpp/network/Packet.hpp>

class Actor;

enum class Stat
{
    Level     = 0x01,
    Xp        = 0x02,
    Str       = 0x03,
    Dex       = 0x04,
    Con       = 0x05,
    Int       = 0x06,
    Wit       = 0x07,
    Men       = 0x08,
    CurHp     = 0x09,
    MaxHp     = 0x0a,
    CurMp     = 0x0b,
    MaxMp     = 0x0c,
    Sp        = 0x0d,
    CurWeight = 0x0e,
    MaxWeight = 0x0f,

    Patk      = 0x11,
    PatkSpeed = 0x12,
    Pdef      = 0x13,
    Evasion   = 0x14,
    Accuracy  = 0x15,
    CritChance = 0x16,
    Matk       = 0x17,
    MatkSpeed  = 0x18,
    Mdef       = 0x19,
    PvpFlag    = 0x1a,
    Karma      = 0x1b,

    CurCp      = 0x21,
    MaxCp      = 0x22,
};

namespace Network::Packet::Server { class StatsUpdatePacket; }

class Network::Packet::Server::StatsUpdatePacket final : public l2cpp::Network::Packet
{
public:
    explicit StatsUpdatePacket(Actor const & emitter);

public:
    StatsUpdatePacket & addStat(Stat id, s32 value);

private:
    void finalizeImpl() override;

private:
    std::unordered_map<Stat, s32> _statValues;
};
