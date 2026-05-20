#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct NpcHtmlMessagePacket; }

struct Network::Packet::Server::NpcHtmlMessagePacket final : public l2cpp::Network::Packet
{
	NpcHtmlMessagePacket(u32 const npcObjectId, std::wstring const& htmlContent);

};