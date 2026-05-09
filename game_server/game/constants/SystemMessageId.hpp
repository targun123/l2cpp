/// @author    Chnossos
/// @date      Created on 2026-04-28

#pragma once

// Project includes
#include "../../Typedefs.hpp"

enum class SystemMessageId : u32
{
    DisconnectedFromServer =    0,
    WelcomeToTheWorldOfL2  =   34,
    EarnedXp               =   45,
    EarnedXpAndSp          =   95,
    YourLevelHasIncreased  =   96,
    EarnedSp               =  331,
    CustomMessage          =  614,
};
