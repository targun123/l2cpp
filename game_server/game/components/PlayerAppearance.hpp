/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "ActorAppearance.hpp"
#include "../constants/Race.hpp"
#include "../constants/Sex.hpp"

class PlayerAppearance : public ActorAppearance
{
public:
    Sex  sex;
    u32  faceId      = 0;
    u32  hairStyleId = 0;
    u32  hairColorId = 0;
    u32  nameColor   = 0xffffff; // white

public:
    explicit PlayerAppearance(Race race = Race::Human, Sex sex_ = Sex::Male);

public:
    auto race() const -> Race;

public:
    void setRace(Race race);

private:
    Race _race;
};
