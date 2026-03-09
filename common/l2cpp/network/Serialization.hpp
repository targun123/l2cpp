/// @author    Chnossos
/// @date      Created on 2026-03-03

#pragma once

// Forward-declare for the macro to work
namespace l2cpp::Network { class Packet; class PacketReader; }

#define DECLARE_PACKET_SERIALIZATION_OPERATOR(T) \
    friend l2cpp::Network::Packet & operator<<(l2cpp::Network::Packet &, T const &)

#define DECLARE_PACKET_DESERIALIZATION_OPERATOR(T) \
    friend l2cpp::Network::PacketReader & operator>>(l2cpp::Network::PacketReader &, T &)

#define DECLARE_PACKET_SERIALIZATION_OPERATORS(T) \
    DECLARE_PACKET_SERIALIZATION_OPERATOR(T);     \
    DECLARE_PACKET_DESERIALIZATION_OPERATOR(T)
