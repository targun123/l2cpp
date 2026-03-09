/// @author    Chnossos
/// @date      Created on 2026-03-09

#include "ItemStorage.hpp"

// Project includes
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <ranges>
#include <unordered_map>

#include "l2cpp/Exception.hpp"

struct ItemStorage::ItemStorageImpl
{
    std::unordered_map<GameObjectId, Item> items;
};

template class Pimpl<ItemStorage::ItemStorageImpl>;

ItemStorage::ItemStorage() = default;
ItemStorage::ItemStorage(ItemStorage &&) noexcept = default;
ItemStorage & ItemStorage::operator=(ItemStorage &&) noexcept = default;
ItemStorage::~ItemStorage() = default;

auto ItemStorage::item(GameObjectId uid) -> OptionalRef<Item>
{
    OptionalRef<Item> result;

    if (_impl->items.contains(uid))
        result.emplace(_impl->items.at(uid));

    return result;
}

auto ItemStorage::item(GameObjectId const uid) const -> OptionalRef<Item const>
{
    OptionalRef<Item const> result;

    if (_impl->items.contains(uid))
        result.emplace(_impl->items.at(uid));

    return result;
}

auto ItemStorage::items() const -> std::vector<Ref<Item const>>
{
    std::vector<Ref<Item const>> result;
    result.append_range(_impl->items | std::views::values);
    return result;
}

auto ItemStorage::add(Item && item) -> Item &
{
    auto const uid      = item.id();
    auto const [it, ok] = _impl->items.try_emplace(uid, std::move(item));
    L2CPP_B_ASSERT(ok, "Failed to insert item '{}' into storage", uid);
    return it->second;
}

auto ItemStorage::remove(Item const & item) -> Item
{
    auto const uid = item.id();
    L2CPP_B_ASSERT(_impl->items.contains(uid), "Cannot remove item '{}' from storage: not found", uid);

    auto result = std::move(_impl->items[uid]);
    _impl->items.erase(uid);
    return result;
}
