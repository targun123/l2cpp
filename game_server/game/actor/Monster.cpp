/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "Monster.hpp"

// Project includes
// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>

struct Monster::MonsterActorImpl
{
};

template class Pimpl<Monster::MonsterActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Monster::Monster() noexcept: Npc(ActorType::Monster) {}
Monster::Monster(Monster &&) noexcept = default;
Monster & Monster::operator=(Monster &&) noexcept = default;
Monster::~Monster() = default;

bool Monster::isAttackable() const
{
    return true;
}
