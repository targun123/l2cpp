/// @author    Chnossos
/// @date      Created on 2026-03-10

#pragma once

// Project includes
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/actor/Monster.hpp"
#include "../game/components/NpcAppearance.hpp"
#include "../game/skill/SkillDirectory.hpp"
#include "../network/packets/server/SkillListPacket.hpp"
#include "../network/packets/server/status/NpcStatusUpdatePacket.hpp"
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

    auto & c = player.currentCharacter()->get();

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
        auto & npc = World::addMonster();
        npc.setName(L"Gremlin");
        npc.setTitle(L"yamete kudasai");
        npc.setPosition(c.position());
        npc.appearance().setId(args.size() >= 2 ? std::stoi(std::wstring(args[1])) : 1);
        npc.appearance().collisionHeight = 15;
        npc.appearance().collisionRadius = 10;

        player.connection().send(NpcStatusUpdatePacket(npc));
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
}
