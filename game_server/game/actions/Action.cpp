/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "Action.hpp"

Action::Action(ActionType const type) noexcept
    : _type(type)
    , _finished(false)
    , _lastUpdateTime(ClockTimePoint::min())
{}

auto Action::type()           const -> ActionType     { return _type;           }
auto Action::isFinished()     const -> bool           { return _finished;       }
auto Action::startTime()      const -> ClockTimePoint { return _startTime;      }
auto Action::lastUpdateTime() const -> ClockTimePoint { return _lastUpdateTime; }

void Action::update(ClockDuration const elapsed, Actor & a)
{
    if (_lastUpdateTime == ClockTimePoint::min()) [[unlikely]]
    {
        _startTime = _lastUpdateTime = std::chrono::steady_clock::now();
        onStarted(a);
    }
    else
    {
        _lastUpdateTime += elapsed;
        updateImpl(elapsed, a);
    }

    if (_finished)
        onFinished(a);
}

void Action::setFinished(bool const finished)
{
    _finished = finished;
}
