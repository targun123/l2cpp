/// @author    Chnossos
/// @date      Created on 2026-04-09

#include "UiGaugePacket.hpp"

using Network::Packet::Server::UiGaugePacket;

UiGaugePacket::UiGaugePacket(GaugeColor const color, MSec const duration)
    : Packet(0x6d)
{
    *this
        << color
        << duration // cur value
        << duration // max value
    ;
}
