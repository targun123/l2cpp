/// @author    Chnossos
/// @date      Created on 2026-04-04

#pragma once

// Project includes
#include "../game/actor/Actor.hpp"

namespace Utils::Maths
{
    auto distance(Position const & pos1, Position const & pos2) -> double;
    inline auto distance(Actor const & a1, Actor const & a2) { return distance(a1.position(), a2.position()); }

    auto isInRange(Position const& pos1, Position const& pos2, double const maxDistance) -> bool;
    inline auto isInRange(Actor const& a1, Actor const& a2, double const maxDistance)
    {
        return isInRange(a1.position(), a2.position(), maxDistance);
    }

}
