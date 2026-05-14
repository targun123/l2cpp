/// @author    Chnossos
/// @date      Created on 2026-03-02

#include "Shortcut.hpp"

// Project includes
#include "../constants/Ui.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/PacketReader.hpp>
#include <l2cpp/utils/Enum.hpp>

struct Shortcut::ShortcutImpl
{
    ShortcutType type  = ShortcutType::None;
    u32          index = Constants::maxShortcuts;
    u32          id    = 0;
    SkillLevel   level = 0;
};

template class Pimpl<Shortcut::ShortcutImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Shortcut::Shortcut() = default;
Shortcut::Shortcut(Shortcut &&) noexcept = default;
Shortcut & Shortcut::operator=(Shortcut &&) noexcept = default;
Shortcut::~Shortcut() = default;

auto Shortcut::isEmpty() const -> bool
{
    return _impl->type  == ShortcutType::None
        || _impl->index >= Constants::maxShortcuts
        || _impl->id    == 0;
}

auto Shortcut::index() const -> std::optional<size_t>
{
    return _impl->index < Constants::maxShortcuts ? std::optional(_impl->index) : std::nullopt;
}

auto Shortcut::type()       const -> ShortcutType { return _impl->type; }
auto Shortcut::targetId()   const -> u32 { return _impl->id; }
auto Shortcut::skillLevel() const -> SkillLevel { return _impl->level; }

void Shortcut::setIndex     (size_t       const idx)   { _impl->index = static_cast<u32>(idx); }
void Shortcut::setType      (ShortcutType const type)  { _impl->type  = type;                  }
void Shortcut::setTargetId  (u32          const id)    { _impl->id    = id;                    }
void Shortcut::setSkillLevel(SkillLevel   const level) { _impl->level = level;                 }

l2cpp::Network::PacketReader & operator>>(l2cpp::Network::PacketReader & r, Shortcut & s)
{
    Shortcut::ShortcutImpl tmp;
    r >> tmp.type >> tmp.index >> tmp.id /* >> u32(1) ?? */;

    L2CPP_B_ASSERT(l2cpp::Utils::Enum::isInContiguousRange(tmp.type, ShortcutType::None, ShortcutType::Count),
                   "Attempt to add a shortcut with invalid type {}", std::to_underlying(tmp.type));
    L2CPP_B_ASSERT(tmp.index < Constants::maxShortcuts,
                   "Attempt to add a shortcut at invalid index {}", tmp.index);

    *s._impl = std::move(tmp);
    return r;
}

l2cpp::Network::Packet & operator<<(l2cpp::Network::Packet & p, Shortcut const & s)
{
    p << s._impl->type << s._impl->index << s._impl->id;

    /**/ if (s._impl->type == ShortcutType::Skill) p << static_cast<u32>(s._impl->level);
    else if (s._impl->type == ShortcutType::Item)  p << 1; // quantity?

    return p;
}
