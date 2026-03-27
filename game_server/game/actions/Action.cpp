/// @author    Chnossos
/// @date      Created on 2026-03-24

#include "Action.hpp"

Action::Action(Type const type) noexcept
    : _type(type)
    , _finished(false)
{}

auto Action::type()           const -> Type           { return _type;           }
auto Action::isFinished()     const -> bool           { return _finished;       }
auto Action::startTime()      const -> ClockTimePoint { return _startTime;      }
auto Action::lastUpdateTime() const -> ClockTimePoint { return _lastUpdateTime; }

void Action::update(ClockDuration const elapsed, Actor & a)
{
    bool const firstTick = _lastUpdateTime == _startTime;

    if (firstTick)
        onStarted(a);

    _lastUpdateTime += elapsed;

    if (!firstTick) [[likely]]
        updateImpl(elapsed, a);

    if (_finished)
        onFinished(a);
}

void Action::setFinished(bool const finished)
{
    _finished = finished;
}
