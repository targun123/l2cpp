/// @author    Chnossos
/// @date      Created on 2026-03-10

#pragma once

// Project includes
#include "_Common.hpp"

// Third-party includes
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

// C++ includes
#include <codecvt>
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

    Packet p(0x0f);
    p
        << 0 // NPC id
        << std::vformat(readWholeFile("data/test.html"), std::make_wformat_args(text))
    ;
    player.connection().send(p);


//     constexpr auto html = LR"(
// <html>
//   <title><font color="C4945A">[Admin Panel]</color></title>
//   <body>
//     <table>
//       <tr><td><h1>Test h1</h1></td></tr>
//       <tr><td><h2>Test h2</h2></td></tr>
//       <tr><td><h3>Test h3</h3></td></tr>
//       <tr><td><h4>Test h4</h4></td></tr>
//       <tr><td><h5>Test h5</h5></td></tr>
//       <tr><td><h6>Test h6</h6></td></tr>
//       <tr><td><h7>Test h7</h7></td></tr>
//     </table>
//     <button>Command inputted: "{}"</button>
//     Input: <edit />
//   </body>
// </html>)"sv;
//
//     Packet p1(0x0f);
//     p1
//         << 0 // NPC id
//         << std::format(html, text)
//     ;
//     player.connection().send(p1);
}
