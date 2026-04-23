/// @author    Chnossos
/// @date      Created on 2026-03-10

#pragma once

// Project includes
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/actor/Monster.hpp"
#include "../game/actor/NpcDirectory.hpp"
#include "../game/components/ActorStatus.hpp"
#include "../game/components/NpcAppearance.hpp"
#include "../game/components/PlayerAppearance.hpp"
#include "../game/components/SkillDirectory.hpp"
#include "../network/packets/server/chat/ChatSystemSayPacket.hpp"
#include "../network/packets/server/skill/SkillListPacket.hpp"
#include "../network/packets/server/status/CharacterStatusUpdateBroadcastPacket.hpp"
#include "../network/packets/server/status/NpcStatusUpdatePacket.hpp"
#include "../utils/Conversion.hpp"
#include "_Common.hpp"

// Third-party includes
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

// C++ includes
#include <format>
#include <fstream>
#include <string_view>

using namespace std::string_view_literals;

static std::wstring readWholeFile(std::string_view path)
{
    std::wstring content;
    std::string  asciiContent;

    if (std::ifstream file(path.data()); file)
    {
        asciiContent.resize(file.seekg(0, std::ios::end).tellg());
        file.seekg(0);
        file.read(asciiContent.data(), asciiContent.size());
    }
    else
    {
        constexpr auto fmt = R"(
<html>
  <body>
    <font color="BF0000">Failed to open "</font><font color="LEVEL">{}</font><font color="BF0000">".</font>
  </body>
</html>
)";
        asciiContent = std::format(fmt, path);
    }

    content.resize(asciiContent.size());
    for (size_t i = 0; auto const c : asciiContent)
        *(reinterpret_cast<byte *>(content.data()) + i++ * 2) = c;

    return content;
}

DEFINE_PACKET_HANDLER(ChatAdminCommand)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    std::wstring text;
    reader >> text;

    auto & c = *player.currentCharacter();

    boost::algorithm::trim_right(text);
    std::vector<std::wstring_view> args;
    boost::algorithm::split(args, text, boost::algorithm::is_any_of(L" "), boost::token_compress_on);

    if (args[0] == L"/")
    {
        Packet p(0x0f);
        p
            << 0 // NPC id
            << std::vformat(readWholeFile("data/test.html"), std::make_wformat_args(text))
        ;
        player.connection().send(p);
    }
    else if (args[0] == L"spawn")
    {
        auto const id = args.size() >= 2 ? std::stoi(std::wstring(args[1])) : 1;

        if (auto const info = NpcDirectory::find(id))
        {
            auto & npc = (info->type == ActorType::Npc ? World::addNpc() : World::addMonster());
            npc.setName(Utils::toWideString(info->name));

            if (info->title.empty())
                npc.setTitle(std::format(L"Lv. {}", npc.status().level()));
            else
                npc.setTitle(Utils::toWideString(info->title));

            npc.setPosition(c.position());
            npc.appearance().setId(id);
            npc.appearance().collisionHeight = 15;
            npc.appearance().collisionRadius = 10;

            World::broadcastAround(c, NpcStatusUpdatePacket(npc), true);
        }
        else
        {
            ChatSystemSayPacket p{614};
            p.appendArg<SysMsgArg::Text>(std::format(L"Failed to spawn npc/mob (id '{}'):", id));
            p.appendArg<SysMsgArg::Text>(L"not found");
            player.connection().send(std::move(p));
        }
    }
    else if (args[0] == L"learn")
    {
        std::wstring cmd;
        SkillId id;
        SkillLevel level = 1;

        std::wistringstream iss(text);
        iss >> cmd >> id >> level;

        auto & skills = c.skills();
        skills.learn(id, level);
        player.connection().send(SkillListPacket(skills));
    }
    else if (args[0] == L"char")
    {
        auto & d = World::addCharacter();
        d.setName(L"dummy");
        d.setPosition(c.position());
        d.appearance().setRace(Race::Elf);
        d.appearance().sex = Sex::Female;
        d.appearance().collisionHeight = 23;
        d.appearance().collisionRadius = 7.5;
        d.setProfession(Profession::ElvenMystic);
        World::broadcastAround(d, CharacterStatusUpdateBroadcastPacket(d));
    }
}
