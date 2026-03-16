/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "NpcAppearance.hpp"

constexpr s32 baseId = 1'000'000;

NpcAppearance::NpcAppearance() noexcept
    : _id(baseId + 1) // Gremlin
    , _titleColor(0)
{}

auto NpcAppearance::id()         const -> u32 { return _id;         }
auto NpcAppearance::titleColor() const -> u32 { return _titleColor; }

void NpcAppearance::setId(u32 const id)            { _id = id < baseId ? baseId + id : id; }
void NpcAppearance::setTitleColor(u32 const color) { _titleColor = color;                  }
