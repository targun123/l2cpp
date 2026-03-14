/// @author    Chnossos
/// @date      Created on 2026-03-11

#include "Skill.hpp"

// Project includes
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <chrono>

using namespace std::chrono;

struct Skill::SkillImpl
{
    SkillInfo info;
    system_clock::time_point lastUse{};
};

template class Pimpl<Skill::SkillImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Skill::Skill(SkillInfo info)
    : _impl(std::move(info))
{}

Skill::Skill(Skill &&) noexcept = default;
Skill & Skill::operator=(Skill &&) noexcept = default;
Skill::~Skill() = default;

auto Skill::info() const -> SkillInfo const &
{
    return _impl->info;
}
