/// @author    Chnossos
/// @date      Created on 2026-03-09

#pragma once

// Project includes
#include "Item.hpp"

#include <l2cpp/Pimpl.hpp>

// C++ includes
#include <vector>

/// Could be a player/pet inventory, a wharehouse, etc.
class ItemStorage
{
public:
    ItemStorage();
    ItemStorage(ItemStorage &&) noexcept;
    ItemStorage & operator=(ItemStorage &&) noexcept;
    virtual ~ItemStorage();

public:
    auto item(GameObjectId uid)       -> OptionalRef<l2::Item>;
    auto item(GameObjectId uid) const -> OptionalRef<l2::Item const>;
    auto items() const -> std::vector<Ref<l2::Item const>>;

public:
    auto add(l2::Item && item) -> l2::Item &;

    /// @return Gives a chance to take back ownership the item instance, else it gets deleted.
    auto remove(l2::Item const &) -> l2::Item;

private:
    struct ItemStorageImpl;
    Pimpl<ItemStorageImpl> _impl;
};
