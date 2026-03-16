/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "PlayerAppearance.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/utils/Enum.hpp>

PlayerAppearance::PlayerAppearance(Race const race, Sex const sex_)
    : sex(sex_)
    , _race(race)
{}

auto PlayerAppearance::race() const -> Race { return _race; }

void PlayerAppearance::setRace(Race const race)
{
    using enum Race;
    L2CPP_B_ASSERT(l2cpp::Utils::Enum::isAnyOf(race, Human, Elf, DarkElf, Orc, Dwarf),
                   "Invalid race '{}' for a playable character", std::to_underlying(race));
    _race = race;
}
