/// @author    Chnossos
/// @date      Created on 2026-03-03

#include "Character.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <array>

using l2::Character;

struct Character::CharacterImpl
{
    std::array<Item, 16> gear;
    std::array<Shortcut, 120> shortcuts{};
};

template class Pimpl<Character::CharacterImpl>;

Character::Character() = default;
Character::~Character() = default;

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;

auto Character::equippedItem(InventoryGearSlot const slot) const -> Item const &
{
    return _impl->gear[std::to_underlying(slot)];
}

auto Character::setShortcut(Shortcut shortcut) -> Shortcut &
{
    L2CPP_B_ASSERT(shortcut.index(), "Cannot set a shortcut whose index ({}) is invalid", *shortcut.index());
    auto const index = *shortcut.index();
    _impl->shortcuts[index] = std::move(shortcut);
    return _impl->shortcuts[index];
}

void Character::delShortcut(size_t const index)
{
    L2CPP_B_ASSERT(index < 120, "Cannot remove a shortcut whose index ({}) is invalid", index);
    _impl->shortcuts[index] = Shortcut();
}
