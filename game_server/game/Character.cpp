/// @author    Chnossos
/// @date      Created on 2026-03-03

#include "Character.hpp"

// Project includes
#include "inventory/Gear.hpp"
#include "inventory/ItemStorage.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <array>
#include <ranges>

using l2::Character;

struct Character::CharacterImpl
{
    ItemStorage inventory;
    Gear gear;
    std::array<Shortcut, 120> shortcuts{};
};

template class Pimpl<Character::CharacterImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Character::Character()
{
    Item formalWear;
    formalWear.tmplate.id       = 6408;
    formalWear.tmplate.name     = "formalWear";
    formalWear.tmplate.category = ItemCategory::Armor;
    formalWear.tmplate.bodyPart = GearSlot::Dress;
    Ref item = _impl->inventory.add(std::move(formalWear));
    _impl->gear.equipItem(item.get());

    Item infinitySpear;
    infinitySpear.tmplate.id       = 6621;
    infinitySpear.tmplate.name     = "infinitySpear";
    infinitySpear.tmplate.category = ItemCategory::Weapon;
    infinitySpear.tmplate.bodyPart = GearSlot::Hands;
    item = _impl->inventory.add(std::move(infinitySpear));
     _impl->gear.equipItem(item.get());

    Item infinityBlade;
    infinityBlade.tmplate.id       = 6611;
    infinityBlade.tmplate.name     = "infinityBlade";
    infinityBlade.tmplate.category = ItemCategory::Weapon;
    infinityBlade.tmplate.bodyPart = GearSlot::RightHand;
    item = _impl->inventory.add(std::move(infinityBlade));
    _impl->gear.equipItem(item.get());

    Item imperialCrusaderShield;
    imperialCrusaderShield.tmplate.id       = 6377;
    imperialCrusaderShield.tmplate.name     = "imperialCrusaderShield";
    imperialCrusaderShield.tmplate.category = ItemCategory::Armor;
    imperialCrusaderShield.tmplate.bodyPart = GearSlot::LeftHand;
    item = _impl->inventory.add(std::move(imperialCrusaderShield));
    _impl->gear.equipItem(item.get());
}

Character::~Character() = default;

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;

auto Character::inventory()       -> ItemStorage       & { return _impl->inventory; }
auto Character::inventory() const -> ItemStorage const & { return _impl->inventory; }

auto Character::gear()       -> Gear       & { return _impl->gear; }
auto Character::gear() const -> Gear const & { return _impl->gear; }

auto Character::setShortcut(Shortcut shortcut) -> Shortcut &
{
    L2CPP_B_ASSERT(shortcut.index(), "Cannot set a shortcut whose index ({}) is invalid", *shortcut.index());
    auto const index = *shortcut.index();
    _impl->shortcuts[index] = std::move(shortcut);
    return _impl->shortcuts[index];
}

void Character::delShortcut(size_t const index)
{
    L2CPP_B_ASSERT(index < _impl->shortcuts.size(), "Cannot remove a shortcut whose index ({}) is invalid", index);
    _impl->shortcuts[index] = Shortcut();
}
