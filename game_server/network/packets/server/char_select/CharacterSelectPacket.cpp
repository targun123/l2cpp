/// @author    Chnossos
/// @date      Created on 2026-03-31

#include "CharacterSelectPacket.hpp"

// Project includes
#include "../../../../Player.hpp"
#include "../../../../game/World.hpp"
#include "../../../../game/actor/Character.hpp"
#include "../../../../game/components/CharacterStatus.hpp"
#include "../../../../game/components/PlayerAppearance.hpp"
#include "../../../../game/components/Position.hpp"
#include "../../../../game/components/Stats.hpp"

using Network::Packet::Server::CharacterSelectPacket;

CharacterSelectPacket::CharacterSelectPacket(Character const & c, u32 const encryptionKey)
    : Packet(0x15)
{
    *this
        << c.name()
        << c.id()
        << c.title()
        << encryptionKey
        << 0 // clanId
        << 0 // ?
        << c.appearance().sex
        << c.appearance().race()
        << c.profession()
        << 1 // active (?)
        << c.position().x
        << c.position().y
        << c.position().z
        << c.status().hp.current
        << c.status().mp.current
        << c.status().sp
        << c.status().xp
        << c.status().level()
        << c.status().karma
        << c.status().pkCount
        << c.stats().INT
        << c.stats().STR
        << c.stats().CON
        << c.stats().MEN
        << c.stats().DEX
        << c.stats().WIT
        << std::array<u32, 32>{} // ?
        << World::inGameTime().count()
        << 0
        << c.profession()
        << std::array<u32, 12>{} // ?
    ;
}
