#include "NpcHtmlMessagePacket.hpp"

using Network::Packet::Server::NpcHtmlMessagePacket;

NpcHtmlMessagePacket::NpcHtmlMessagePacket() : Packet(0x0f, "NpcHtmlMessage")
{
	*this
		<< 10001
		<< L"<html><body>My Text is missing:<br> TEXT </body></html>"
		<< 0
	;
}