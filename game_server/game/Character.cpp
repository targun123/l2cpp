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
    std::array<Shortcut, 120> shortcuts{};

// public:
    auto addItemToInventory() -> Item &;
    void setWeapon(Item * item);

    auto itemAt(InventoryGearSlot slot) const -> Item *;
};

template class Pimpl<Character::CharacterImpl>;

auto Character::CharacterImpl::addItemToInventory() -> Item &
{
    return inventory.emplace_back();
}

void Character::CharacterImpl::setWeapon(Item * item)
{
    if (item)
    {
        gear[std::to_underlying(InventoryGearSlot::RightWeapon)] = item;
        if (item->tmplate.bodyPart == BodyPart::Hands)
            gear[std::to_underlying(InventoryGearSlot::LeftWeapon)] = item;
        else
            gear[std::to_underlying(InventoryGearSlot::LeftWeapon)] = nullptr;

        item->equipped = true;
    }
    else
    {
        if (Item *& weapon = gear[std::to_underlying(InventoryGearSlot::RightWeapon)]; weapon)
        {
            weapon->equipped = false;
            weapon = nullptr;
        }

        if (Item *& weapon = gear[std::to_underlying(InventoryGearSlot::LeftWeapon)]; weapon)
        {
            weapon->equipped = false;
            weapon = nullptr;
        }
    }
}

auto Character::CharacterImpl::itemAt(InventoryGearSlot slot) const -> Item *
{
    return gear[std::to_underlying(slot)];
}

Character::Character()
{
    auto & weapon = _impl->addItemToInventory();

    // TMP
    weapon.tmplate.id       = 6621; // Infinity Spear
    weapon.tmplate.category = ItemCategory::Weapon;
    weapon.tmplate.bodyPart = BodyPart::Hands;
    _impl->setWeapon(&weapon);
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

    if (auto const item = _impl->itemAt(slot); item)
        ref = *item;

    return ref;
}

auto Character::equipItem(GameObjectId const uid) -> PairOf<OptionalRef<Item const>>
{
    PairOf<OptionalRef<Item const>> result;

    // Look up if already equipped
    for (Item *& item : _impl->gear)
    {
        if (item && item->id() == uid)
            return result;
    }

    auto const it = std::ranges::find_if(_impl->inventory, [uid] (auto const & item) { return item.id() == uid; });
    if (it != _impl->inventory.end())
    {
        if (auto const oldItem = unequipItem(it->tmplate.bodyPart); oldItem)
            result.first = oldItem;

        _impl->setWeapon(&*it);
        result.second = *it;
    }

    return result;
}

auto Character::equipItem(Item const & item) -> OptionalRef<Item const>
{
    return std::nullopt;
}

auto Character::unequipItem(BodyPart bodyPart) -> OptionalRef<Item const>
{
    OptionalRef<Item const> ref;
    switch (bodyPart)
    {
        using enum BodyPart;
        case None:          break;
        case Underwear:     break;
        case RightEar:      break;
        case LeftEar:       break;
        case Ears:          break;
        case Neck:          break;
        case RightFinger:   break;
        case LeftFinger:    break;
        case Fingers:       break;
        case Head:          break;
        case LeftHand:      break;
        case Gloves:        break;
        case Chest:         break;
        case Legs:          break;
        case Feet:          break;
        case Back:          break;
        case RightHand:
        case Hands:
        {
            if (auto const item = _impl->itemAt(InventoryGearSlot::RightWeapon); item)
            {
                _impl->setWeapon(nullptr);
                ref = *item;
            }
            break;
        }
        case TwoPieceArmor: break;
        case Hair:          break;
        case Dress:         break;
    }
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
