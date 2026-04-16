/// @author    Chnossos
/// @date      Created on 2026-04-14

#include "ActorSkillEffectSystem.hpp"

// Project includes
#include "../../network/packets/server/status/AbnormalEffectListPacket.hpp"
#include "../World.hpp"
#include "utils/Enum.hpp"

/// Update the effect list if we remove any non-damage effect, or it's damage over time
static bool needToUpdateList(bool const currentValue, AbnormalEffect const & effect)
{
    using enum AbnormalEffectType;
    return currentValue                                             // short-circuit
        || l2cpp::Utils::Enum::isAnyOf(effect.type(), Buff, Debuff) // Non hp modifying effect
        || effect.duration() != ClockDuration::zero();              // is damage or heal over time (it has an icon)
}

void ActorSkillEffectSystem::updateImpl(ClockDuration const elapsed, Actor & actor)
{
    bool updateList = false;

    auto & effects = actor.abnormalEffects();
    for (auto it = effects.begin(); it != effects.end(); )
    {
        if ((*it)->elapsed() == ClockDuration::zero()) // Effect just got applied
            updateList = needToUpdateList(updateList, **it);

        if ((*it)->update(elapsed))
        {
            updateList = needToUpdateList(updateList, **it);
            it = effects.erase(it);
        }
        else
            ++it;
    }

    if (updateList)
        World::send(actor, Network::Packet::Server::AbnormalEffectListPacket{actor});
}
