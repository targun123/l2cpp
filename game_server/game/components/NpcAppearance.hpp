/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "ActorAppearance.hpp"

class NpcAppearance : public ActorAppearance
{
public:
    NpcAppearance() noexcept;

public:
    auto id() const -> u32;
    auto titleColor() const -> u32;

public:
    void setId(u32 id);
    void setTitleColor(u32 color);

private:
    u32 _id, _titleColor;
};
