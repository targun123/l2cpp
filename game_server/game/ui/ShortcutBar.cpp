/// @author    Chnossos
/// @date      Created on 2026-05-18

#include "ShortcutBar.hpp"

// Project includes
#include <l2cpp/Exception.hpp>

ShortcutBar::ShortcutBar()
    : _readOnlyShortcuts(_shortcuts.size())
{}

auto ShortcutBar::size() const -> size_t
{
    return _shortcuts.size();
}

auto ShortcutBar::at(Shortcut::Index const index) const -> OptRef<Shortcut>
{
    checkIndex(index);

    return _shortcuts[index] ? OptRef(*_shortcuts[index]) : std::nullopt;
}

auto ShortcutBar::shortcuts() const -> std::span<OptRef<Shortcut const> const>
{
    return _readOnlyShortcuts;
}

void ShortcutBar::unset(Shortcut::Index const index)
{
    checkIndex(index);

    _shortcuts        [index].reset();
    _readOnlyShortcuts[index].reset();
}

void ShortcutBar::checkIndex(Shortcut::Index const index) const
{
    L2CPP_B_ASSERT(index < _shortcuts.size(), "Invalid index '{}': out of range", index);
}

void ShortcutBar::setImpl(Shortcut::Index const index, std::unique_ptr<Shortcut> shortcut)
{
    if (auto const & s = _shortcuts[index] = std::move(shortcut))
        _readOnlyShortcuts[index] = *s;
    else
        _readOnlyShortcuts[index].reset();
}

