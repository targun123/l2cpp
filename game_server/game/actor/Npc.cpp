/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "Npc.hpp"

// Project includes
#include "../components/ActorStatus.hpp"
#include "../components/NpcAppearance.hpp"

Npc::Npc(u32 const id)
    : Npc(ActorType::Npc, id)
{}

Npc::Npc(ActorType const type, u32 const id)
    : Actor(type)
{
    addComponent<ActorStatus>();
    auto & appearance = addComponent<NpcAppearance>();
    appearance.setId(id);
}

auto Npc::appearance()       -> NpcAppearance       & { return component<NpcAppearance>(); }
auto Npc::appearance() const -> NpcAppearance const & { return component<NpcAppearance>(); }
auto Npc::status()     const -> ActorStatus   const & { return component<ActorStatus>();   }

