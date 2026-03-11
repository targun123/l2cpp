/// @author    Chnossos
/// @date      Created on 2026-03-02

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Serialization.hpp>

// C++ includes
#include <optional>

class Shortcut
{
    friend class Character;

    DECLARE_PACKET_SERIALIZATION_OPERATORS(Shortcut);

public:
    enum class Type : u32 { None, Item, Skill, Action, Macro, Recipe, Count };

public:
    Shortcut();
    Shortcut(Shortcut &&) noexcept;
    Shortcut & operator=(Shortcut &&) noexcept;
    ~Shortcut();

public:
    auto index()      const -> std::optional<size_t>;
    auto type()       const -> Type;
    auto targetId()   const -> u32;
    auto skillLevel() const -> SkillLevel;

private:
    void setSkillLevel(SkillLevel level);

private:
    struct ShortcutImpl;
    Pimpl<ShortcutImpl> _impl;
};
