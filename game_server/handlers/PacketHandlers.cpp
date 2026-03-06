/// @author    Chnossos
/// @date      Created on 2026-02-24

#include "PacketHandlers.hpp"

// Project includes
#include "../network/packets/client/OpCodes.hpp"

namespace Handler
{
#define HANDLER(name) void handle ## name(Player &);
    HANDLER(Authenticate)
    HANDLER(CharacterCancelDeletion)
    HANDLER(CharacterCreate)
    HANDLER(CharacterSelect)
    HANDLER(CharacterShowCreationScreen)
    HANDLER(ChatSay)
    HANDLER(ConnectionClosing)
    HANDLER(EnterWorld)
    HANDLER(InventoryOpen)
    HANDLER(ItemUnequip)
    HANDLER(ItemUse)
    HANDLER(LeaveWorld)
    HANDLER(ManorList)
    HANDLER(MiniMapShow)
    HANDLER(Move)
    HANDLER(MoveUpdate)
    HANDLER(Protocol)
    HANDLER(QuestList)
    HANDLER(ShortcutBarAdd)
    HANDLER(ShortcutBarRemove)
    HANDLER(SkillList)
    HANDLER(SkillUse)
    HANDLER(TargetClear)
    HANDLER(TargetSelect)
#undef HANDLER
}

std::unordered_map<byte, std::pair<PacketHandler, std::string_view>> gPacketHandlers
{
#define HANDLER(name) {std::to_underlying(Network::ClientOpCode::name), {&Handler::handle ## name, #name}}
    HANDLER(Authenticate),
    HANDLER(CharacterCancelDeletion),
    HANDLER(CharacterCreate),
    HANDLER(CharacterSelect),
    HANDLER(CharacterShowCreationScreen),
    HANDLER(ChatSay),
    HANDLER(ConnectionClosing),
    HANDLER(EnterWorld),
    HANDLER(InventoryOpen),
    HANDLER(ItemUnequip),
    HANDLER(ItemUse),
    HANDLER(LeaveWorld),
    HANDLER(ManorList),
    HANDLER(MiniMapShow),
    HANDLER(Move),
    HANDLER(MoveUpdate),
    HANDLER(Protocol),
    HANDLER(QuestList),
    HANDLER(ShortcutBarAdd),
    HANDLER(ShortcutBarRemove),
    HANDLER(SkillList),
    HANDLER(SkillUse),
    HANDLER(TargetClear),
    HANDLER(TargetSelect),
#undef HANDLER
};
