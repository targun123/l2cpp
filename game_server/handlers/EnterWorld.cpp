/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/inventory/InventoryListPacket.hpp"

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

    assign(c.finalStats.pAtkSpeed,     c.baseStats.pAtkSpeed     * c.finalStats.pAtkSpeedMutliplier);
    assign(c.finalStats.runSpeed,      c.baseStats.runSpeed      * c.finalStats.moveSpeedMutliplier);
    assign(c.finalStats.walkSpeed,     c.baseStats.walkSpeed     * c.finalStats.moveSpeedMutliplier);
    assign(c.finalStats.swimRunSpeed,  c.baseStats.swimRunSpeed  * c.finalStats.moveSpeedMutliplier);
    assign(c.finalStats.swimWalkSpeed, c.baseStats.swimWalkSpeed * c.finalStats.moveSpeedMutliplier);
    assign(c.finalStats.flyRunSpeed,   c.baseStats.flyRunSpeed   * c.finalStats.moveSpeedMutliplier);
    assign(c.finalStats.flyWalkSpeed,  c.baseStats.flyWalkSpeed  * c.finalStats.moveSpeedMutliplier);
}

DEFINE_PACKET_HANDLER(EnterWorld)
{
    auto & c = player.currentCharacter()->get();
    player.connection().send(InventoryListPacket(false, c.inventory()));

    updateFinalStats(c);
    player.connection().send(CharacterStatusUpdatePacket(c));
}
