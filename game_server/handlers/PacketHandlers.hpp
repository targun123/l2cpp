/// @author    Chnossos
/// @date      Created on 2026-02-24

#pragma once

// C++ includes
#include <string_view>
#include <unordered_map>

class Player;

using PacketHandler = void(*)(Player &);

extern std::unordered_map<unsigned char, std::pair<PacketHandler, std::string_view>> gPacketHandlers;
