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
    Npc();
    explicit Npc(ActorType type);

public:
    auto appearance()       -> NpcAppearance       &;
    auto appearance() const -> NpcAppearance const &;

    virtual auto status() const -> ActorStatus const &;

    bool isAttackable() const override { return false; }
};
