/// @author    Chnossos
/// @date      Created on 2026-05-09

#include "Characters.hpp"

// Project includes
#include "../game/actor/Character.hpp"
#include "../game/components/CharacterStatus.hpp"
#include "../game/components/Position.hpp"
#include "../utils/Conversion.hpp"

#include <l2cpp/services/Database.hpp>

void Orm::saveCharacter(Character const & c)
{
    SQLite::Statement query(Database::instance(), R"(
        UPDATE
            characters
        SET
            title       = :title
          , pos_x       = :pos_x
          , pos_y       = :pos_y
          , pos_z       = :pos_z
          , orientation = :orientation
        WHERE
            name LIKE :name
    )");
    query.bind(":name",        Utils::toString(c.name()));
    query.bind(":title",       Utils::toString(c.title()));
    query.bind(":pos_x",       c.position().x);
    query.bind(":pos_y",       c.position().y);
    query.bind(":pos_z",       c.position().z);
    query.bind(":orientation", c.position().orientation);
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to save character");

    query = SQLite::Statement(Database::instance(), R"(
        UPDATE
            character_professions
        SET
            xp = :xp
          , sp = :sp
        WHERE
            character_id = (SELECT id FROM characters WHERE name LIKE :name LIMIT 1)
            AND profession = :profession
    )");
    query.bind(":xp",         c.status().xp());
    query.bind(":sp",         c.status().sp());
    query.bind(":name",       Utils::toString(c.name()));
    query.bind(":profession", std::to_underlying(c.profession()));
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to save character's profession");
}

void Orm::loadCharacter(Character & c)
{
    // Here we do not load the preview fields again, we load the rest in order to enter the World
    SQLite::Statement query(Database::instance(), R"(
        SELECT
            title
          , orientation
        FROM
            characters
        WHERE
            name LIKE :name
    )");
    query.bind(":name", Utils::toString(c.name()));
    L2CPP_B_ASSERT(query.executeStep(), "Failed to load character");

    c.setTitle(Utils::toWideString(query.getColumn("title").getString()));
    c.setOrientation(query.getColumn("orientation").getInt());
}
