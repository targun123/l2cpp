/// @author    Chnossos
/// @date      Created on 2026-04-04

#include "Maths.hpp"

// Project includes
#include "../game/components/Position.hpp"

auto Utils::Maths::distance(Position const & pos1, Position const & pos2) -> double
{
    return std::hypot(static_cast<double>(pos1.x - pos2.x),
                      static_cast<double>(pos1.y - pos2.y),
                      static_cast<double>(pos1.z - pos2.z));
}

auto Utils::Maths::isInRange(Position const& pos1, Position const& pos2, double const maxDistance) -> bool
{
    double const dx = static_cast<double>(pos1.x - pos2.x);
    double const dy = static_cast<double>(pos1.y - pos2.y);
    double const dz = static_cast<double>(pos1.z - pos2.z);

    double const squaredDistance = (dx * dx) + (dy * dy) + (dz * dz);

    return squaredDistance <= (maxDistance * maxDistance);
}
