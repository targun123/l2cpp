/// @author    Chnossos
/// @date      Created on 2026-04-21

#include "ClientExitGamePacket.hpp"

using namespace Network::Packet::Server;

ClientExitGamePacket::ClientExitGamePacket()
    : Packet(0x7e, "ClientExitGame")
{}
