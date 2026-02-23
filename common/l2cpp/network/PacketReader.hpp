/// @author    Chnossos
/// @date      Created on 2026-02-23

#pragma once

// Project includes
#include "../Typedefs.hpp"

// C++ includes
#include <span>
#include <string>

namespace l2cpp::network { class PacketReader; }

class l2cpp::network::PacketReader
{
public:
    explicit PacketReader(std::span<byte const> packet) noexcept: cursor(std::move(packet)) {}

public:
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
    PacketReader & operator>>(T & t)
    {
        std::memcpy(&t, cursor.data(), sizeof(t));
        cursor = cursor.subspan(sizeof(T));
        return *this;
    }

    template<typename C>
    PacketReader & operator>>(std::basic_string<C> & str)
    {
        str = reinterpret_cast<C const *>(cursor.data());
        cursor = cursor.subspan(str.size() * sizeof(C) + sizeof(C));
        return *this;
    }

private:
    std::span<byte const> cursor;
};
