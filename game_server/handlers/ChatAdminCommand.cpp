/// @author    Chnossos
/// @date      Created on 2026-03-10

#pragma once

// Project includes
#include "../game/Character.hpp"
#include "_Common.hpp"

// Third-party includes
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

// C++ includes
#include <format>
#include <fstream>
#include <string_view>

#include "../game/skill/SkillDirectory.hpp"
#include "../network/packets/server/SkillListPacket.hpp"
#include "../utils/Conversion.hpp"

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
    <font color="BF0000">Failed to open path "</font><font color="C4945A">{}</font><font color="BF0000">".</font>
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

        Packet p(0x16); // NpcInfo
        p
            << 42 // GameObjectId
            << 1'000'001 // display ID
            << 1 // attackable
            << c.pos.x
            << c.pos.y
            << c.pos.z
            << 0 // head angle
            << 0 // ?
            << 100 // castSpeed
            << 100 // pAtkSpeed
            << 50  // runSpeed
            << 20  // walkSpeed
            << 50  // swim runSpeed
            << 20  // swim walkSpeed
            << 50  // fly runSpeed
            << 20  // fly walkSpeed
            << 50  // fly runSpeed
            << 20  // fly walkSpeed
            << 1.0 // move multiplier
            << 1.0 // atkSpeed multiplier
            << 10. // collision radius
            << 15. // collision height
            << 0 // right hand
            << 0 // chest
            << 0 // left hand
            << true // name above char
            << true // isRunning
            << false // isInCombat
            << false // is like dead
            << false // isSummonned
            << L"Gremlin" // name
            << L"lv. 1" // title
            << 0 // title color 0=default
            << 0 // not pvp flagged
            << 0 // no karma
            << 0 // no visual effect
            << 0 // no clan
            << 0 // no clan crest
            << 0 // no ally
            << 0 // no ally crest
            << false // not in water nor flying
            << u16(0) // no team
            << 10. // collision radius
            << 15. // collision height
            << 0 // weapon enchant level
        ;
        player.connection().send(p);
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
