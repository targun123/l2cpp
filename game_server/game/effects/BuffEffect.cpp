/// @author    Chnossos
/// @date      Created on 2026-04-24

#include "BuffEffect.hpp"

// Project includes
#include "../../network/packets/server/status/AbnormalEffectListPacket.hpp"
#include "../../network/packets/server/status/CharacterStatusUpdateBroadcastPacket.hpp"
#include "../../network/packets/server/status/CharacterStatusUpdatePacket.hpp"
#include "../actor/Character.hpp"
#include "../components/Stats.hpp"
#include "../World.hpp"

BuffEffect::BuffEffect(
    Actor               & source
  , Actor               & target
  , SkillUid      const   skillUid
  , ClockDuration const   duration
  , StatId        const   modifiedStat
  , StatValue     const   value
)
    : AbnormalEffect(AbnormalEffectType::Buff, source, target, skillUid, duration)
    , _modifiedStat(modifiedStat)
    , _value(value)
{}

void BuffEffect::onStarted()
{
    modifyStat(_value);
}

void BuffEffect::onFinished()
{
    modifyStat(-_value);
}

void BuffEffect::modifyStat(StatValue const newValue) const
{
    auto & stats = *target().component<Stats>();
    stats[_modifiedStat] += newValue;
    stats.compute(target());

    namespace SC   = Network::Packet::Server;
    auto const & c = static_cast<Character &>(target());

    World::send           (target(), Network::Packet::Server::AbnormalEffectListPacket{target()});
    World::send           (target(), SC::CharacterStatusUpdatePacket{c});
    World::broadcastAround(target(), SC::CharacterStatusUpdateBroadcastPacket{c});
}

