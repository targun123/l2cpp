/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "../constants/ActionType.hpp"

#include <l2cpp/Typedefs.hpp>

class Actor;

class Action
{
public:
    explicit Action(ActionType type) noexcept;
    Action(Action const &) noexcept = default;
    Action & operator=(Action const &) noexcept = default;
    Action(Action &&) noexcept = default;
    Action & operator=(Action &&) noexcept = default;
    virtual ~Action() noexcept = default;

public:
    auto type()           const -> ActionType;
    auto isFinished()     const -> bool;
    auto startTime()      const -> ClockTimePoint;
    auto lastUpdateTime() const -> ClockTimePoint;

public:
    virtual bool canBeInterrupted() const = 0;

public:
    void update(ClockDuration elapsed, Actor &);

protected:
    void setFinished(bool finished);

private:
    virtual void onStarted(Actor &) {}
    virtual void updateImpl(ClockDuration, Actor &) = 0;
    virtual void onFinished(Actor &) {}

private:
    ActionType _type;
    bool _finished;
    ClockTimePoint _startTime, _lastUpdateTime;
};
