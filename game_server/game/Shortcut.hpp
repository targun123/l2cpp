/// @author    Chnossos
/// @date      Created on 2026-03-02

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Serialization.hpp>

// C++ includes
#include <optional>

namespace l2 { class Shortcut; }

class l2::Shortcut : public l2cpp::Network::Serializable
{
    ENABLE_STREAM_SERIALIZATION(Shortcut);

public:
    enum class Type : u32 { None, Item, Skill, Action, Macro, Recipe, Count };

public:
    Shortcut();
    Shortcut(Shortcut &&) noexcept;
    Shortcut & operator=(Shortcut &&) noexcept;
    ~Shortcut() override;

public:
    auto index() const -> std::optional<size_t>;

private:
    auto serialize  (l2cpp::Network::Packet &) const -> l2cpp::Network::Packet &       override;
    auto deserialize(l2cpp::Network::PacketReader &) -> l2cpp::Network::PacketReader & override;

private:
    struct ShortcutImpl;
    Pimpl<ShortcutImpl> _impl;
};
