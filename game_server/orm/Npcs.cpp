
#include "Npcs.hpp"
// Project includes
#include <l2cpp/services/Database.hpp>
#include "../game_server/utils/Conversion.hpp"
void Orm::loadNpcHtmls(std::unordered_map<u64, std::wstring>& htmls)
{
    SQLite::Statement query(Database::instance(), R"(
        SELECT
            (SELECT COUNT(*) FROM npc_htmls) AS size
          , *
        FROM
            npc_htmls
    )");

    while (query.executeStep())
    {
        htmls.reserve(query.getColumn("size").getUInt());

        u64 const templateId = query.getColumn("template_id").getUInt();
        u64 const page = query.getColumn("page").getUInt();
        std::string const htmlText = query.getColumn("html_text").getString();

        u64 const key = (templateId << 32) | page;

        
        std::wstring const wHtmlText = Utils::toWideString(htmlText);

        htmls.try_emplace(key, wHtmlText);
    }
}