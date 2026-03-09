/// @author    Chnossos
/// @date      Created on 2026-03-09

#pragma once

// Project includes
#include "GearTransaction.hpp"

#include <l2cpp/Pimpl.hpp>

/// Handles the items that the character currently wears
class Gear
{
public:
    Gear();
    Gear(Gear &&) noexcept;
    Gear & operator=(Gear &&) noexcept;
    ~Gear();

public:
    auto item(l2::GearSlot slot)       -> OptionalRef<l2::Item>;
    auto item(l2::GearSlot slot) const -> OptionalRef<l2::Item const>;

    bool hasActiveWeapon() const;
    auto weapon() const -> OptionalRef<l2::Item const>;

public:
    auto equipItem  (l2::Item & item) -> GearTransaction;
    auto unequipItem(l2::Item & item) -> GearTransaction;

private:
    struct GearImpl;
    Pimpl<GearImpl> _impl;
};
