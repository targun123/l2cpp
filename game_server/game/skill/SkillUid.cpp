/// @author    Chnossos
/// @date      Created on 2026-03-31

#include "SkillUid.hpp"

// Project includes
#include <l2cpp/network/Packet.hpp>

DEFINE_PACKET_SERIALIZATION_OPERATOR(SkillUid, uid)
{
    return p << static_cast<u32>(uid.id()) << static_cast<u32>(uid.level());
}
