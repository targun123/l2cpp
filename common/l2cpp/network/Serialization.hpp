/// @author    Chnossos
/// @date      Created on 2026-03-03

#pragma once

// Forward-declare for the macro to work
namespace l2cpp::Network { class Packet; class PacketReader; struct Serializable; }

struct l2cpp::Network::Serializable
{
    Serializable()                                 noexcept = default;
    Serializable(Serializable const & )            noexcept = default;
    Serializable & operator=(Serializable const &) noexcept = default;
    Serializable(Serializable &&)                  noexcept = default;
    Serializable & operator=(Serializable &&)      noexcept = default;
    virtual ~Serializable()                        noexcept = default;

    virtual auto serialize  (Packet       &) const -> Packet       & = 0;
    virtual auto deserialize(PacketReader &)       -> PacketReader & = 0;
};

#define ENABLE_STREAM_SERIALIZATION(T)                                                                                 \
    friend l2cpp::Network::PacketReader & operator>>(l2cpp::Network::PacketReader & r, T & c){return c.deserialize(r);}\
    friend l2cpp::Network::Packet       & operator<<(l2cpp::Network::Packet & p, T const & c){return c.serialize(p);  }
