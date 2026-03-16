/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "Actor.hpp"

class NonPlayableActor : public Actor
{
public:
    NonPlayableActor();

public:
    auto appearance() -> u32;
    virtual bool isAttackable() = 0;

public:

};
