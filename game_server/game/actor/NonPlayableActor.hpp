/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "Actor.hpp"

class NpcAppearance;

class NonPlayableActor : public Actor
{
public:
    NonPlayableActor();

public:
    auto appearance() const -> NpcAppearance const &;
    virtual bool isAttackable() = 0;

public:

};
