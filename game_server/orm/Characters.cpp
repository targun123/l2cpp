/// @author    Chnossos
/// @date      Created on 2026-05-09

#include "Characters.hpp"

// Project includes
#include "../game/actor/Character.hpp"
#include "../utils/Conversion.hpp"

#include <l2cpp/services/Database.hpp>

void Orm::saveCharacter(Character const & c)
{
    SQLite::Statement query(Database::instance(), R"(
        UPDATE
            characters
        SET
            title = :title
        WHERE
            name LIKE :name
    )");

    query.bind(":name",  Utils::toString(c.name()));
    query.bind(":title", Utils::toString(c.title()));
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to save character");
}

void Orm::loadCharacter(Character & c)
{
    // Here we do not load the preview parameters again
    SQLite::Statement query(Database::instance(), R"(
        SELECT
            title
        FROM
            characters
        WHERE
            name LIKE :name
    )");
    query.bind(":name", Utils::toString(c.name()));
    L2CPP_B_ASSERT(query.executeStep(), "Failed to load character");

    c.setTitle(Utils::toWideString(query.getColumn("title").getString()));
}
