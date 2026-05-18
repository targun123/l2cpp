/// @author    Chnossos
/// @date      Created on 2026-04-09

#pragma once

// Project includes
#include "../../../../Typedefs.hpp"

#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct UiGaugePacket; }

enum class GaugeColor : u32
{
    Blue,  ///< Used during skills
    Red,   ///< Used during bow auto-attacks
    Cyan,  ///< Used underwater
    Green, ///< Used to show mount's hunger
};

struct Network::Packet::Server::UiGaugePacket final : public l2cpp::Network::Packet
{
    UiGaugePacket(GaugeColor color, ClockDuration duration);
};
