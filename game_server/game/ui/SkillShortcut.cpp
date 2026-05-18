/// @author    Chnossos
/// @date      Created on 2026-05-18

#include "SkillShortcut.hpp"

// Project includes
#include <l2cpp/network/Packet.hpp>

SkillShortcut::SkillShortcut(Index const index, SkillUid const skillUid)
    : Shortcut{index, ShortcutType::Skill}
    , _skillUid{skillUid}
{}

auto SkillShortcut::targetId()  const -> u32                { return _skillUid.id();    }
auto SkillShortcut::extraInfo() const -> std::optional<u32> { return _skillUid.level(); }

void SkillShortcut::serializeImpl(l2cpp::Network::Packet & p) const
{
    p
        << static_cast<u32>(_skillUid.id())
        << static_cast<u32>(_skillUid.level())
    ;
}
