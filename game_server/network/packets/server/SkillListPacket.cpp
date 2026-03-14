/// @author    Chnossos
/// @date      Created on 2026-03-11

#include "SkillListPacket.hpp"

// C++ includes
#include <ranges>

using Network::Packet::Server::SkillListPacket;

SkillListPacket::SkillListPacket(SkillRegistry const & skills)
    : Packet(0x58)
{
    *this << static_cast<u32>(skills.size());

    for (auto const & skill : skills | std::views::values)
        *this << skill.info().type() << static_cast<u32>(skill.info().level()) << static_cast<u32>(skill.info().id());
}
