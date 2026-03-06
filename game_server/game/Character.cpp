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
// private:
    std::array<Item *, std::to_underlying(InventoryGearSlot::Count)> gear{};
    std::list<Item> inventory;
    ;
    std::array<Shortcut, 120> shortcuts{};

// public:
    auto addItemToInventory() -> Item &;
    void setWeapon(Item & item);
};

template class Pimpl<Character::CharacterImpl>;

auto Character::CharacterImpl::addItemToInventory() -> Item &
{
    return inventory.emplace_back();
}

void Character::CharacterImpl::setWeapon(Item & item)
{
    gear[std::to_underlying(InventoryGearSlot::RightWeapon)] = &item;
    if (item.tmplate.bodyPart == BodyPart::Hands)
        gear[std::to_underlying(InventoryGearSlot::LeftWeapon)] = &item;

    item.equipped = true;
}

Character::Character()
{
    auto & weapon           = _impl->addItemToInventory();

    // TMP
    weapon.tmplate.id       = 6621; // Infinity Spear
    weapon.tmplate.category = ItemCategory::Weapon;
    weapon.tmplate.bodyPart = BodyPart::Hands;

    _impl->setWeapon(weapon);
}

Character::~Character() = default;

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;

auto Character::inventory() const -> std::vector<std::reference_wrapper<Item const>>
{
    return {_impl->inventory.cbegin(), _impl->inventory.cend()};
}

auto Character::equippedItem(InventoryGearSlot const slot) const -> OptionalRef<Item const>
{
    OptionalRef<Item const> ref;

    if (auto const item = _impl->gear[std::to_underlying(slot)]; item)
        ref = *item;

    return ref;
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
