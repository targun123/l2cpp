/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "NonPlayableActor.hpp"

// Project includes
#include "../components/ActorStatus.hpp"
#include "../components/NpcAppearance.hpp"

NonPlayableActor::NonPlayableActor(ActorType const type)
    : Actor(type)
{
    addComponent<ActorStatus>();
    addComponent<NpcAppearance>();
}

auto NonPlayableActor::appearance()       -> NpcAppearance       & { return component<NpcAppearance>(); }
auto NonPlayableActor::appearance() const -> NpcAppearance const & { return component<NpcAppearance>(); }

auto NonPlayableActor::status() const -> ActorStatus const & { return component<ActorStatus>(); }

