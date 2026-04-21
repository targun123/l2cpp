/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actions/AttackAction.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/Gear.hpp"
#include "../game/components/Position.hpp"
#include "../game/components/Stats.hpp"
#include "../network/packets/server/ActionFailedPacket.hpp"
#include "../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../network/packets/server/target/TargetMonsterSelectPacket.hpp"
#include "../network/packets/server/target/TargetSelectPacket.hpp"

DEFINE_PACKET_HANDLER(ActionRequest)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot select target, no current character");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 targetId;
    Position origin;
    bool disallowMovement; // 0=click 1=shift+click
    reader >> targetId >> origin >> disallowMovement;

    auto & character = *player.currentCharacter();
    if (!character.target() || character.target()->id() != targetId) // No current target or trying to change target?
    {
        if (auto const action = character.currentAction(); !action || action->type() != ActionType::Skill)
        {
            /**/ if (auto const c = World::character(targetId))
            {
                character.setTarget(c);
                player.connection().send(TargetSelectPacket(character, c));
                World::subscribeToTarget(c, character);
                return;
            }
            else if (auto const m = World::monster(targetId))
            {
                character.setTarget(m);
                player.connection().send(TargetMonsterSelectPacket(character, m));
                World::subscribeToTarget(m, character);

                StatsUpdatePacket p(m);
                p.addStat(Stat::MaxHp, m->stats()[StatId::MaxHp]);
                p.addStat(Stat::CurHp, m->stats()[StatId::CurHp]);
                player.connection().send(p);
                return;
            }
        }
    }
    else if (targetId != character.id()) // second request on target other than self, launch attack!
    {
        character.state = ActorState::Attacking;
        character.doNext<AttackAction>(character.target(), character.stats()[StatId::PAtkSpeed]);
        return;
    }

    player.connection().send(ActionFailedPacket{});
}
