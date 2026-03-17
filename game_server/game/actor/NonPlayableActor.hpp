/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "Actor.hpp"

class ActorStatus;
class NpcAppearance;

class NonPlayableActor : public Actor
{
public:
    NonPlayableActor();

public:
    auto appearance()       -> NpcAppearance       &;
    auto appearance() const -> NpcAppearance const &;

    virtual auto status() const -> ActorStatus const &;

    virtual bool isAttackable() const = 0;
};
