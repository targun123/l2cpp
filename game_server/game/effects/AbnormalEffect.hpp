/// @author    Chnossos
/// @date      Created on 2026-04-14

#pragma once

// Project includes
#include "../skill/SkillUid.hpp"

class Actor;

enum class AbnormalEffectType
{
    Buff,
    Debuff,
    Damage,
    Heal,
};

class AbnormalEffect
{
public:
    AbnormalEffect(AbnormalEffectType type,
                   Actor &            target,
                   SkillUid           skillUid,
                   ClockDuration      effectDuration         = ClockDuration::zero(),
                   ClockDuration      tickDuration           = ClockDuration::zero(),
                   ClockDuration      initialTriggerDuration = ClockDuration::zero());
    virtual ~AbnormalEffect();

public:
    auto type()              const -> AbnormalEffectType;
    auto skillUid()          const -> SkillUid;
    auto duration()          const -> ClockDuration;
    auto elapsed()           const -> ClockDuration;
    auto remainingDuration() const -> ClockDuration;

protected:
    auto target() const -> Actor &;

public:
    /// @returns Whether the effect needs to be removed after this update.
    bool update(ClockDuration elapsed);

protected:
    void setFinished(bool finished);

private:
    virtual void onStarted()  {}
    virtual void onTick()     {}
    virtual void onFinished() {}

private:
    AbnormalEffectType _type;
    Actor &            _target;
    SkillUid           _skillUid;
    ClockDuration      _duration, _elapsed, _elapsedSinceLastTick, _tickDuration, _initialTriggerDuration;
    bool               _finished;
};

enum class DamageElementType
{
    Neutral,
    Earth,
    Wind,
    Fire,
    Water,
    Poison,
    Bleed,
};

class DamageEffect : public AbnormalEffect
{
public:
    explicit DamageEffect(Actor &           target,
                          SkillUid          skillUid,
                          DamageElementType type,
                          double            damage,
                          ClockDuration     effectDuration         = ClockDuration::zero(),
                          ClockDuration     tickDuration           = ClockDuration::zero(),
                          ClockDuration     initialTriggerDuration = ClockDuration::zero());

private:
    void onTick() override;

private:
    DamageElementType _elementType;
    double            _damage;
    ClockDuration     _initialTriggerDuration;
    ClockDuration     _tick, _elapsedSinceLastTick;
};

class BuffEffect : public AbnormalEffect
{
public:
    BuffEffect(Actor & target, SkillUid skillUid, ClockDuration duration);

private:
    void onStarted() override;
};

/*
{
  "Wind Walk": {
    "id": 1204,
    "levels": 2,
    "category": "active/magic",
    "target_type": "single/character",
    "cast_duration": 4000,
    "cooldown": 6000,
    "cast_range": 400,
    "effect_range": 900,
    "routes": {
      "1~2": [{
        "type": "buff",
        "stat": "move_speed_flat",
        "power": [20, 33],
        "duration": 1200,
        "mp_cost": [20, 27],
        "mp_initial_cost": [4, 6]
      }],
      "101~130": [{
        "type": "buff",
        "stat": "move_speed_flat",
        "power": 33,
        "duration": [1240, 1280, 1320, 1360, 1400, 1440, 1480, 1520, 1560, 1600, 1640, 1680, 1720, 1760, 1800,
                     1840, 1880, 1920, 1960, 2000, 2040, 2080, 2120, 2160, 2200, 2240, 2280, 2320, 2360, 2400],
        "mp_cost": 27,
        "mp_initial_cost": 6
      }],
      "141~170": [{
        "type": "buff",
        "stat": "move_speed_flat",
        "power": 33,
        "duration": 1200,
        "mp_cost": [20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15,
                    15, 15, 14, 14, 14, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10],
        "mp_initial_cost": [5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4,
                            4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3]
      }]
    }
  }
}
*/
