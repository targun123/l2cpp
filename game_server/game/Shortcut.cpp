/// @author    Chnossos
/// @date      Created on 2026-03-02

#include "Shortcut.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/PacketReader.hpp>

#include <fmt/ostream.h>

namespace
{
    template<typename E> requires std::is_enum_v<E>
    constexpr bool isInRange(E index, E start, E end)
    {
        return std::to_underlying(index) >= std::to_underlying(start)
            && std::to_underlying(index)  < std::to_underlying(end);
    }
}

struct l2::Shortcut::ShortcutImpl
{
    Type type = Type::None;
    u32 index = static_cast<u32>(-1);
    u32 id    = static_cast<u32>(-1);
    u32 level = 4;
};

template class Pimpl<l2::Shortcut::ShortcutImpl>;

// ---------------------------------------------------------------------------------------------------------------------

l2::Shortcut::Shortcut() = default;
l2::Shortcut::Shortcut(Shortcut &&) noexcept = default;
l2::Shortcut & l2::Shortcut::operator=(Shortcut &&) noexcept = default;
l2::Shortcut::~Shortcut() = default;

auto l2::Shortcut::index() const -> std::optional<size_t>
{
    std::optional<size_t> idx;

    if (_impl->index < 120)
        idx = _impl->index;

    return idx;
}

l2cpp::Network::PacketReader & l2::operator>>(l2cpp::Network::PacketReader & r, Shortcut & s)
{
    Shortcut::ShortcutImpl tmp;
    r >> tmp.type >> tmp.index >> tmp.id;

    L2CPP_B_ASSERT(isInRange(tmp.type, Shortcut::Type::None, Shortcut::Type::Count),
                   "Attempt to add a shortcut with invalid type {}", std::to_underlying(tmp.type));
    L2CPP_B_ASSERT(tmp.index < 120,
                   "Attempt to add a shortcut at invalid index {}", tmp.index);

    *s._impl = std::move(tmp);
    return r;
}

l2cpp::Network::Packet & l2::operator<<(l2cpp::Network::Packet & p, Shortcut const & s)
{
    p << s._impl->type << s._impl->index << s._impl->id;

    /**/ if (s._impl->type == Shortcut::Type::Skill) p << s._impl->level;
    else if (s._impl->type == Shortcut::Type::Item)  p << 1; // quantity?

    return p;
}
