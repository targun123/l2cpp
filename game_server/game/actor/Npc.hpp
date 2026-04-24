/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "Actor.hpp"

class ActorStatus;
class NpcAppearance;

class Npc : public Actor
{
public:
    explicit Npc(u32 id);

protected:
    Npc(ActorType type, u32 id);

public:
    auto appearance()       -> NpcAppearance       &;
    auto appearance() const -> NpcAppearance const &;

    virtual auto status() const -> ActorStatus const &;

    bool isAttackable() const override { return false; }
};
