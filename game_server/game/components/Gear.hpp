/// @author    Chnossos
/// @date      Created on 2026-03-09

#pragma once

// Project includes
#include "../ecs/Component.hpp"
#include "../inventory/GearTransaction.hpp"

#include <l2cpp/Pimpl.hpp>

/// Handles the items that the character currently wears
class Gear : public Component
{
public:
    Gear();
    Gear(Gear &&) noexcept;
    Gear & operator=(Gear &&) noexcept;
    ~Gear() override;

public:
    auto item(GearSlot slot)       -> OptionalRef<Item>;
    auto item(GearSlot slot) const -> OptionalRef<Item const>;

    auto itemId        (GearSlot slot) const -> u32;
    auto itemTemplateId(GearSlot slot) const -> u32;

    bool hasActiveWeapon() const;
    auto weapon() const -> OptionalRef<Item const>;

public:
    auto equipItem  (Item & item) -> GearTransaction;
    auto unequipItem(Item & item) -> GearTransaction;

private:
    struct GearImpl;
    Pimpl<GearImpl> _impl;
};
