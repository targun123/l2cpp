/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "Npc.hpp"

// Project includes
#include "../components/ActorStatus.hpp"
#include "../components/NpcAppearance.hpp"

Npc::Npc()
    : Npc(ActorType::Npc)
{}

Npc::Npc(ActorType const type)
    : Actor(type)
{
    addComponent<ActorStatus>();
    addComponent<NpcAppearance>();
}

auto Npc::appearance()       -> NpcAppearance       & { return component<NpcAppearance>(); }
auto Npc::appearance() const -> NpcAppearance const & { return component<NpcAppearance>(); }
auto Npc::status()     const -> ActorStatus   const & { return component<ActorStatus>();   }

