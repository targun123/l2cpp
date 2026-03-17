/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "Monster.hpp"

// Project includes
#include <l2cpp/details/Pimpl.hpp>

struct Monster::MonsterActorImpl
{
    u32 level = 1;
};

template class Pimpl<Monster::MonsterActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Monster::Monster() = default;
Monster::Monster(Monster &&) noexcept = default;
Monster & Monster::operator=(Monster &&) noexcept = default;
Monster::~Monster() = default;

bool Monster::isAttackable() const
{
    return true;
}

auto Monster::level() const -> u32
{
    return _impl->level;
}

void Monster::setLevel(u32 const level)
{
    L2CPP_B_ASSERT(0 < level && level <= 80, "Invalid monster level '{}' (should be between [ 1 ; 80 ])", level);
    _impl->level = level;
}
