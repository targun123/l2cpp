/// @author    Chnossos
/// @date      Created on 2026-03-14

#include "SkillDirectory.hpp"

// Project includes
#include "../skill/SkillTemplateDirectory.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <unordered_map>

struct SkillDirectory::SkillDirectoryImpl
{
    std::unordered_map<SkillId, Skill> skills;
};

template class Pimpl<SkillDirectory::SkillDirectoryImpl>;

// ---------------------------------------------------------------------------------------------------------------------

SkillDirectory::SkillDirectory() = default;
SkillDirectory::SkillDirectory(SkillDirectory &&) noexcept = default;
SkillDirectory & SkillDirectory::operator=(SkillDirectory &&) noexcept = default;
SkillDirectory::~SkillDirectory() = default;

auto SkillDirectory::size() const -> size_t
{
    return _impl->skills.size();
}

auto SkillDirectory::skill(SkillId const id) -> OptRef<Skill>
{
    OptRef<Skill> skill;

    if (auto const it = _impl->skills.find(id); it != _impl->skills.end())
        skill = it->second;

    return skill;
}

auto SkillDirectory::skill(SkillId const id) const -> OptRef<Skill const>
{
    return *const_cast<SkillDirectory *>(this)->skill(id);
}

auto SkillDirectory::learn(SkillId const id, SkillLevel const level) -> Skill &
{
    if (auto const s = skill(id); s)
        s->setLevel(level);
    else
    {
        auto const tmpl = SkillTemplateDirectory::skill(id, level);
        L2CPP_B_ASSERT(tmpl, "Failed to learn skill '{}' at level '{}': not loaded", id, level);
        _impl->skills.try_emplace(id, *tmpl);
    }
    return _impl->skills.at(id);
}

auto SkillDirectory::begin()        -> std::unordered_map<SkillId, Skill>::iterator       { return _impl->skills.begin();  }
auto SkillDirectory::end()          -> std::unordered_map<SkillId, Skill>::iterator       { return _impl->skills.end();    }
auto SkillDirectory::begin()  const -> std::unordered_map<SkillId, Skill>::const_iterator { return _impl->skills.cbegin(); }
auto SkillDirectory::end()    const -> std::unordered_map<SkillId, Skill>::const_iterator { return _impl->skills.cend();   }
auto SkillDirectory::cbegin() const -> std::unordered_map<SkillId, Skill>::const_iterator { return _impl->skills.cbegin(); }
auto SkillDirectory::cend()   const -> std::unordered_map<SkillId, Skill>::const_iterator { return _impl->skills.cend();   }
