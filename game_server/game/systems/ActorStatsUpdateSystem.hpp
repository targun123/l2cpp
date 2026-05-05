/// @author    Chnossos
/// @date      Created on 2026-05-04

#pragma once

// Project includes
#include "../ecs/ActorSystem.hpp"

class Character;
struct CharacterStatus;
class Npc;

struct ActorStatsUpdateSystem : public ActorSystem
{
    void updateImpl(ClockDuration elapsed, Actor &) override;

private:
    void updateCharacterStats(Character &, Stats const & oldStats) const;
    void updateNpcStats(Npc &, Stats const & oldStats) const;
};
