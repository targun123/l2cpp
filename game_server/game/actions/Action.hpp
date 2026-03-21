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
    // Action(Type const type, ClockTimePoint const startTime_) noexcept
    //     : _type(type), _startTime(startTime_), _lastUpdateTime(startTime_)
    // {}
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
    void restart() { /*_startTime = _lastUpdateTime = std::chrono::steady_clock::now();*/ _lastUpdateTime = decltype(_lastUpdateTime){};}
    void update(ClockDuration const elapsed) { _lastUpdateTime += elapsed; }

private:
    Type _type;
    ClockTimePoint _startTime, _lastUpdateTime;
};
