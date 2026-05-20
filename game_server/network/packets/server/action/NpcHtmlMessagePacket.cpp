#include "NpcHtmlMessagePacket.hpp"

using Network::Packet::Server::NpcHtmlMessagePacket;

NpcHtmlMessagePacket::NpcHtmlMessagePacket(u32 const npcObjectId, std::wstring const& htmlContent) : Packet(0x0f, "NpcHtmlMessage")
{
	

	*this
		<< npcObjectId
		<< htmlContent
		<< 0
	;
}