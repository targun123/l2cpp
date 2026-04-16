/// @author    Chnossos
/// @date      Created on 2026-04-16

#include "AbnormalEffectListPacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"
#include "../../../../game/skill/SkillUid.hpp"

using Network::Packet::Server::AbnormalEffectListPacket;

AbnormalEffectListPacket::AbnormalEffectListPacket(Actor const & actor)
    : Packet(0x7f, "AbnormalEffectList")
{
    auto const & effects = actor.abnormalEffects();

    std::vector<std::pair<SkillUid, ClockDuration>> uniqueSkillEffects;
    uniqueSkillEffects.reserve(effects.size());

    for (auto const & effect : effects)
        uniqueSkillEffects.emplace_back(std::make_pair(effect->skillUid(), effect->remainingDuration()));

    *this
        << static_cast<u16>(effects.size())
    ;

    for (auto const [skillUid, remainingDuration] : uniqueSkillEffects)
    {
        *this
            << skillUid
            << std::chrono::floor<std::chrono::seconds>(remainingDuration)
        ;
    }
}
