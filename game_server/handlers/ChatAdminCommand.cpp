/// @author    Chnossos
/// @date      Created on 2026-03-10

#pragma once

// Project includes
#include "../game/actor/Character.hpp"
#include "../game/actor/Monster.hpp"
#include "../game/components/NpcAppearance.hpp"
#include "../game/skill/SkillDirectory.hpp"
#include "../network/packets/server/SkillListPacket.hpp"
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

    boost::algorithm::trim_right(text);

    if (text == L"/")
    {
        Packet p(0x0f);
        p
            << 0 // NPC id
            << std::vformat(readWholeFile("data/test.html"), std::make_wformat_args(text))
        ;
        player.connection().send(p);
    }
    else if (text.starts_with(L"spawn"))
    {
        auto const & c = player.currentCharacter()->get();

        /*  <stats str="40" int="21" dex="30" wit="20" con="43" men="10">
            <vitals hp="39.74519" hpRegen="2" mp="40" mpRegen="0.9" />
            <attack physical="8.47458" magical="5.78704" random="30" critical="4" accuracy="4.75" attackSpeed="253" type="SWORD" range="40" distance="80" width="120" />
            <defence physical="44.44444" magical="29.5916164000214" />
            <speed>
            <walk ground="20" />
            <run ground="50" />
            </speed>
            <hitTime>370</hitTime>
            </stats>
            <collision>
                <radius normal="10" />
                <height normal="15" />
            </collision>
        */

        Monster gremlin;
        gremlin.setName(L"Gremlin");
        gremlin.setTitle(L"yamete kudasai");
        gremlin.setPosition(c.position());

        auto & appearance = gremlin.component<NpcAppearance>();
        appearance.setId(1);
        appearance.collisionHeight = 15;
        appearance.collisionRadius = 10;

        player.connection().send(NpcStatusUpdatePacket(gremlin));
    }
    else if (text.starts_with(L"learn "))
    {
        std::wstring cmd;
        SkillId id;
        SkillLevel level = 1;

        std::wistringstream iss(text);
        iss >> cmd >> id >> level;

        auto & skills = player.currentCharacter()->get().skills();
        skills.learn(id, level);
        player.connection().send(SkillListPacket(skills));
    }
}
