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

DEFINE_PACKET_HANDLER(ActionRequest) try
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot select target, no current character");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 targetId;
    Position origin;
    bool disallowMovement;
    reader >> targetId >> origin >> disallowMovement;

    auto & c = *player.currentCharacter();
    if (!c.target() || c.target()->id() != targetId) // No current target or trying to change target?
    {
        auto & target = World::subscribeToTarget(targetId, c);
        c.setTarget(target);

        if (target.type() != ActorType::Monster)
            c.player->connection().send(TargetSelectPacket{c, target});
        else
        {
            c.player->connection().send(TargetMonsterSelectPacket{c, static_cast<Monster const &>(target)});

            StatsUpdatePacket p(target);
            p.addStat(Stat::MaxHp, target.stats()[StatId::MaxHp]);
            p.addStat(Stat::CurHp, target.stats()[StatId::CurHp]);
            c.player->connection().send(std::move(p));
        }
    }
    else if (targetId != c.id()) // second request on target other than self, launch attack!
    {
        c.state = ActorState::Attacking;
        c.doNext<AttackAction>(c.target(), c.stats()[StatId::PAtkSpeed]);
    }
    else
        player.connection().send(ActionFailedPacket{});
}
catch (...)
{
    player.connection().send(ActionFailedPacket{});
    throw;
}
