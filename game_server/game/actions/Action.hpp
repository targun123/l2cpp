/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

class Action
{
public:
    enum class Type
    {
        Move, Attack
    };

public:
    explicit Action(Type const type) noexcept: _type(type) {}
    Action(Action const &) noexcept = default;
    Action & operator=(Action const &) noexcept = default;
    Action(Action &&) noexcept = default;
    Action & operator=(Action &&) noexcept = default;
    virtual ~Action() noexcept = default;

public:
    Type type() const { return _type; }
    auto startTime() const { return _startTime; }
    auto lastUpdateTime() const { return _lastUpdateTime; }

public:
    void restart() { _lastUpdateTime = ClockTimePoint{}; }
    void update(ClockDuration const elapsed) { _lastUpdateTime += elapsed; }

private:
    Type _type;
    ClockTimePoint _startTime, _lastUpdateTime;
};
