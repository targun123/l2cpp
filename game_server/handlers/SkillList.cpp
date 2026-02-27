/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(SkillList)
{
    Packet p(0x58);
    p << 1 // count
      << 0 // isPassive
      << 4 // level
      << 7029 // id (Super Haste)
    ;
    player.connection().send(p);
}
