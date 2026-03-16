/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "NonPlayableActor.hpp"

// Project includes
#include "../components/NpcAppearance.hpp"

NonPlayableActor::NonPlayableActor()
{
    addComponent<NpcAppearance>();
}
