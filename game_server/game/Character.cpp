/// @author    Chnossos
/// @date      Created on 2026-03-03

#include "Character.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <array>
#include <ranges>

using l2::Character;

struct Character::CharacterImpl
{
    std::unordered_map<GearSlot, OptionalRef<Item>> gear;
    std::list<Item> inventory;
    std::array<Shortcut, 120> shortcuts{};

    auto addItemToInventory() -> Item &;

    auto itemAt(GameObjectId uid)    -> OptionalRef<Item>;
    auto itemAt(GearSlot slot)       -> OptionalRef<Item> &;
    auto itemAt(GearSlot slot) const -> OptionalRef<Item const>;

    /// Apply a function to a slot and all its dependant slots
    template<typename Func>
    auto applyToDependentSlots(GearSlot slot, Func apply);

    auto equipItem  (Item &) -> GearTransaction;
    auto unequipItem(Item const &, OptionalRef<GearTransaction> = std::nullopt) -> GearTransaction;
};

template class Pimpl<Character::CharacterImpl>;

auto Character::CharacterImpl::addItemToInventory() -> Item &
{
    return inventory.emplace_back();
}

auto Character::CharacterImpl::itemAt(GameObjectId uid) -> OptionalRef<Item>
{
    OptionalRef<Item> item;

    auto const it = std::ranges::find_if(inventory, [uid] (auto const & item) { return item.id() == uid; });
    if (it != std::end(inventory))
        item = *it;

    return item;
}

auto Character::CharacterImpl::itemAt(GearSlot const slot) -> OptionalRef<Item> &
{
    L2CPP_B_ASSERT(slot != GearSlot::None, "Tried to reference gear item @ slot None");
    switch (slot)
    {
        case GearSlot::Hands:         return gear[GearSlot::RightHand];
        case GearSlot::Fingers:       return gear[GearSlot::RightFinger];
        case GearSlot::TwoPieceArmor: return gear[GearSlot::Chest];
        case GearSlot::Dress:         return gear[GearSlot::Chest];
        default:                      return gear[slot];
    }
}

auto Character::CharacterImpl::itemAt(GearSlot const slot) const -> OptionalRef<Item const>
{
    return const_cast<CharacterImpl *>(this)->itemAt(slot);
}

template<typename Func>
auto Character::CharacterImpl::applyToDependentSlots(GearSlot slot, Func apply)
{
    std::vector<GearSlot> slots;
    slots.reserve(4);

    switch (slot)
    {
        using enum GearSlot;
        case None:                                                                                break;
        case Hands:         slots.append_range(std::initializer_list{LeftHand, RightHand});       break;
        case Fingers:       slots.append_range(std::initializer_list{LeftFinger, RightFinger});   break;
        case TwoPieceArmor: slots.append_range(std::initializer_list{Chest, Legs});               break;
        case Dress:         slots.append_range(std::initializer_list{Chest, Legs, Gloves, Feet}); break;
        default:            slots.emplace_back(slot);                                             break;
    }

    for (auto const s : slots)
        std::invoke(apply, gear[s]);
}

auto Character::CharacterImpl::equipItem(Item & item) -> GearTransaction
{
    GearTransaction transaction {.target = item};

    auto const slot = item.tmplate.bodyPart;
    if (slot == GearSlot::None) // Unequippable item
    {
        transaction.succeeded = false;
        return transaction;
    }

    // TODO: verify whether conditions are met to avoid unequipping when equipping is not possible

    if (auto const & curItem = itemAt(slot))
    {
        // First, unequip all slots used by the item currently located where we want to equip.
        // This is needed in case the new item uses less slots than the current item.
        // E.g.: putting on gloves (1 slot) should remove Formal Wear (4 slots).
        applyToDependentSlots(curItem->get().tmplate.bodyPart, [this, &transaction] (OptionalRef<Item> const & itemSlot)
        {
            if (itemSlot.has_value())
                unequipItem(itemSlot->get(), transaction);
        });

        // Second, also unequip all slots our new item is going to equip to.
        // This is needed when the new item uses more slots than the current item located where we want to equip.
        // E.g.: Formal Wear occupying multiple armor slots.
        applyToDependentSlots(slot, [this, &transaction] (OptionalRef<Item> const & itemSlot)
        {
            if (itemSlot.has_value())
                unequipItem(itemSlot->get(), transaction);
        });
    }

    if (transaction.succeeded)
    {
        // Everything went right. Now truly unequip old items
        for (auto const & itemRef : transaction.oldItems | std::views::values)
        {
            applyToDependentSlots(itemRef.get().tmplate.bodyPart, [] (OptionalRef<Item> & itemSlot)
            {
                itemSlot->get().equipped = false;
                itemSlot.reset();
            });
        }

        // Finally, equip the item onto all dependent slots
        applyToDependentSlots(slot, [&item] (OptionalRef<Item> & itemSlot) { itemSlot = item; });
        item.equipped = true;
        transaction.curItem = item;
    }

    return transaction;
}

auto Character::CharacterImpl::unequipItem(Item const & item, OptionalRef<GearTransaction> const curTransaction)
    -> GearTransaction
{
    GearTransaction t{.target = item};
    auto & transaction = curTransaction ? curTransaction->get() : t;

    // First, verify that provided item is the one that's currently equipped
    auto const   slot    = item.tmplate.bodyPart;
    auto const & curItem = itemAt(slot);
    transaction.succeeded = transaction.succeeded // Do not proceed if current transaction has already failed
                            && curItem && curItem->get().id() == item.id();

    if (!transaction.succeeded)
        return transaction;

    // TODO: verify whether conditions are met before unequipping

    applyToDependentSlots(slot, [&transaction] (OptionalRef<Item> const & itemSlot)
    {
        if (itemSlot.has_value())
            transaction.oldItems.try_emplace(itemSlot->get().id(), *itemSlot);
    });

    // Now that we're about to finish with the transaction, apply if everything succeeded and we're not a subtransaction
    if (!curTransaction)
    {
        for (auto const & itemRef : transaction.oldItems | std::views::values)
        {
            applyToDependentSlots(itemRef.get().tmplate.bodyPart, [] (OptionalRef<Item> & itemSlot)
            {
                itemSlot->get().equipped = false;
                itemSlot.reset();
            });
        }
    }

    return transaction;
}

// ---------------------------------------------------------------------------------------------------------------------

Character::Character()
{
    auto & formalWear           = _impl->addItemToInventory();
    formalWear.tmplate.id       = 6408;
    formalWear.tmplate.category = ItemCategory::Armor;
    formalWear.tmplate.bodyPart = GearSlot::Dress;
    _impl->equipItem(formalWear);

    auto & infinitySpear           = _impl->addItemToInventory();
    infinitySpear.tmplate.id       = 6621;
    infinitySpear.tmplate.category = ItemCategory::Weapon;
    infinitySpear.tmplate.bodyPart = GearSlot::Hands;
    // _impl->equipItem(infinitySpear);

    auto & infinityBlade           = _impl->addItemToInventory();
    infinityBlade.tmplate.id       = 6611;
    infinityBlade.tmplate.category = ItemCategory::Weapon;
    infinityBlade.tmplate.bodyPart = GearSlot::RightHand;
    _impl->equipItem(infinityBlade);

    auto & imperialCrusaderShield  = _impl->addItemToInventory();
    imperialCrusaderShield.tmplate.id = 6377;
    imperialCrusaderShield.tmplate.category = ItemCategory::Armor;
    imperialCrusaderShield.tmplate.bodyPart = GearSlot::LeftHand;
    _impl->equipItem(imperialCrusaderShield);
}

Character::~Character() = default;

Character::Character(Character &&) noexcept = default;
Character & Character::operator=(Character &&) noexcept = default;

auto Character::inventory() const -> std::vector<std::reference_wrapper<Item const>>
{
    return {_impl->inventory.cbegin(), _impl->inventory.cend()};
}

auto Character::gearItem(GearSlot const slot) const -> OptionalRef<Item const>
{
    return _impl->itemAt(slot);
}

bool Character::isEquipped(Item const & item) const
{
    auto const gearItem = _impl->itemAt(item.tmplate.bodyPart);
    return gearItem && gearItem->get().id() == item.id();
}

bool Character::hasActiveWeapon() const
{
    return weapon().has_value();
}

auto Character::weapon() const -> OptionalRef<Item const>
{
    return _impl->itemAt(GearSlot::RightHand);
}

auto Character::equipItem(Item const & item) -> GearTransaction
{
    return _impl->equipItem(_impl->itemAt(item.id())->get());
}

auto Character::unequipItem(Item const & item) -> GearTransaction
{
    return _impl->unequipItem(_impl->itemAt(item.id())->get());
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
    L2CPP_B_ASSERT(index < _impl->shortcuts.size(), "Cannot remove a shortcut whose index ({}) is invalid", index);
    _impl->shortcuts[index] = Shortcut();
}
