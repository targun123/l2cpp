/// @author    Chnossos
/// @date      Created on 2026-03-02

#include "Shortcut.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/PacketReader.hpp>
#include <l2cpp/utils/Enum.hpp>

namespace Constants
{
    constexpr size_t maxShortcuts = 120;
}

struct Shortcut::ShortcutImpl
{
    Type type = Type::None;
    u32 index = static_cast<u32>(-1);
    u32 id    = static_cast<u32>(-1);
    u32 level = 4;
};

template class Pimpl<Shortcut::ShortcutImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Shortcut::Shortcut() = default;
Shortcut::Shortcut(Shortcut &&) noexcept = default;
Shortcut & Shortcut::operator=(Shortcut &&) noexcept = default;
Shortcut::~Shortcut() = default;

auto Shortcut::index() const -> std::optional<size_t>
{
    std::optional<size_t> idx;

    if (_impl->index < Constants::maxShortcuts)
        idx = _impl->index;

    return idx;
}

l2cpp::Network::PacketReader & operator>>(l2cpp::Network::PacketReader & r, Shortcut & s)
{
    Shortcut::ShortcutImpl tmp;
    r >> tmp.type >> tmp.index >> tmp.id;

    L2CPP_B_ASSERT(l2cpp::utils::isInContiguousRange(tmp.type, Shortcut::Type::None, Shortcut::Type::Count),
                   "Attempt to add a shortcut with invalid type {}", std::to_underlying(tmp.type));
    L2CPP_B_ASSERT(tmp.index < Constants::maxShortcuts,
                   "Attempt to add a shortcut at invalid index {}", tmp.index);

    *s._impl = std::move(tmp);
    return r;
}

l2cpp::Network::Packet & operator<<(l2cpp::Network::Packet & p, Shortcut const & s)
{
    p << s._impl->type << s._impl->index << s._impl->id;

    /**/ if (s._impl->type == Shortcut::Type::Skill) p << s._impl->level;
    else if (s._impl->type == Shortcut::Type::Item)  p << 1; // quantity?

    return p;
}
