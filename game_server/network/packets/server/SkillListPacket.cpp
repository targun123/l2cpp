/// @author    Chnossos
/// @date      Created on 2026-03-11

#include "SkillListPacket.hpp"

// C++ includes
#include <ranges>

using Network::Packet::Server::SkillListPacket;

SkillListPacket::SkillListPacket(SkillDirectory const & skills)
    : Packet(0x58)
{
    *this << static_cast<u32>(skills.size());

    for (auto const & skill : skills | std::views::values)
    {
        *this
            << (skill.tmplate().type() == SkillType::Passive ? 1 : 0)
            << static_cast<u32>(skill.tmplate().level())
            << static_cast<u32>(skill.tmplate().id())
        ;
    }
}
