/// @author    Chnossos
/// @date      Created on 2026-02-24

#include "PacketHandlers.hpp"

namespace Handler
{
#define HANDLER(name) void handle ## name(Player &);
    HANDLER(Protocol)
    HANDLER(Authenticate)
    HANDLER(CharacterCancelDeletion)
    HANDLER(CharacterCreate)
    HANDLER(CharacterList)
    HANDLER(CharacterSelect)
    HANDLER(CharacterShowCreationScreen)
    HANDLER(ConnectionClosing)
    HANDLER(EnterWorld)
    HANDLER(LeaveWorld)
    HANDLER(ManorList)
    HANDLER(MiniMapShow)
    HANDLER(Move)
    HANDLER(MoveUpdate)
    HANDLER(QuestList)
    HANDLER(ShortcutBarAdd)
    HANDLER(ShortcutBarRemove)
    HANDLER(SkillList)
    HANDLER(SkillUse)
    HANDLER(TargetClear)
    HANDLER(TargetSelect)
#undef HANDLER
}

std::unordered_map<unsigned char, std::pair<PacketHandler, std::string_view>> gPacketHandlers
{
#define HANDLER(opCode, name) { opCode, {&Handler::handle ## name, #name} }
    HANDLER(0x00, Protocol),
    HANDLER(0x08, Authenticate),
    HANDLER(0x01, Move),
    HANDLER(0x03, EnterWorld),
    HANDLER(0x04, TargetSelect),
    HANDLER(0x09, ConnectionClosing),
    HANDLER(0x0b, CharacterCreate),
    HANDLER(0x0d, CharacterSelect),
    HANDLER(0x0e, CharacterShowCreationScreen),
    HANDLER(0x2f, SkillUse),
    HANDLER(0x33, ShortcutBarAdd),
    HANDLER(0x35, ShortcutBarRemove),
    HANDLER(0x37, TargetClear),
    HANDLER(0x3f, SkillList),
    HANDLER(0x46, LeaveWorld),
    HANDLER(0x48, MoveUpdate),
    HANDLER(0x62, CharacterCancelDeletion),
    HANDLER(0x63, QuestList),
    HANDLER(0xcd, MiniMapShow),
    HANDLER(0xd0, ManorList),
#undef HANDLER
};
