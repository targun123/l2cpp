/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actions/AttackAction.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/ActorStatus.hpp"
#include "../game/components/Gear.hpp"
#include "../game/components/Stats.hpp"
#include "../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../network/packets/server/target/TargetMonsterSelectPacket.hpp"
#include "../network/packets/server/target/TargetSelectPacket.hpp"

DEFINE_PACKET_HANDLER(ActionRequest)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot select target, no current character");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 targetId;
    s32 originX, originY, originZ;
    u8 actionId; // 0=click 1=shift+click
    reader >> targetId >> originX >> originY >> originZ >> actionId;

    auto & character = *player.currentCharacter();

    // No current target or current target is different from requested?
    if (!character.target() || character.target()->id() != targetId)
    {
        /**/ if (auto const c = World::character(targetId))
        {
            character.setTarget(*c);
            player.connection().send(TargetSelectPacket(character, c));
        }
        else if (auto const m = World::monster(targetId))
        {
            character.setTarget(m);
            player.connection().send(TargetMonsterSelectPacket(character, m));

            World::subscribeToTarget(m, character);

            StatsUpdatePacket p(m);
            p.addStat(Stat::MaxHp, static_cast<u32>(m->stats().maxHp));
            p.addStat(Stat::CurHp, static_cast<u32>(m->stats().curHp));
            player.connection().send(p);
        }
    }
    else // second request on target, launch attack!
    {
        character.state = ActorState::Attacking;
        character.doNext<AttackAction>(*character.target(), character.stats().pAtkSpeed);
    }
}
