/// @author    Chnossos
/// @date      Created on 2026-03-02

#pragma once

// Project includes
#include "../../Typedefs.hpp"
#include "../constants/ShortcutType.hpp"

#include <l2cpp/Pimpl.hpp>
#include <l2cpp/network/Serialization.hpp>

class Shortcut
{
    DECLARE_PACKET_SERIALIZATION_OPERATORS(Shortcut);

public:
    Shortcut();
    Shortcut(Shortcut &&) noexcept;
    Shortcut & operator=(Shortcut &&) noexcept;
    ~Shortcut();

public:
    auto isEmpty()    const -> bool;
    auto index()      const -> std::optional<size_t>;
    auto type()       const -> ShortcutType;
    auto targetId()   const -> u32;
    auto skillLevel() const -> SkillLevel;

public:
    void setIndex(size_t);
    void setType(ShortcutType);
    void setTargetId(u32);
    void setSkillLevel(SkillLevel);

private:
    struct ShortcutImpl;
    Pimpl<ShortcutImpl> _impl;
};
