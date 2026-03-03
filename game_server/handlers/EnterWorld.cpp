/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"

template<typename T, typename F>
void assign(T & t, F f)
{
    t = static_cast<T>(f);
}

static void updateFinalStats(l2::Character & c)
{
    // First update race stats from gear & buffs

    // Then update dependent stats
    assign(c.finalStats.pAtkSpeedMutliplier, c.baseStats.pAtkSpeedMutliplier + (c.finalStats.DEX - 20) / 100.);
    assign(c.finalStats.moveSpeedMutliplier, c.baseStats.moveSpeedMutliplier + (c.finalStats.DEX - 20) / 100.);
}

DEFINE_PACKET_HANDLER(EnterWorld)
{
    auto & c = player.currentCharacter()->get();
    updateFinalStats(c);
    player.connection().send(Network::Packet::Server::CharacterStatusUpdatePacket(c));
}
