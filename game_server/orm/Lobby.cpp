/// @author    Chnossos
/// @date      Created on 2026-05-06

#include "Lobby.hpp"

// Project includes
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/CharacterSelectionData.hpp"
#include "../game/components/CharacterStatus.hpp"
#include "../game/components/PlayerAppearance.hpp"
#include "../game/components/Position.hpp"
#include "../utils/Conversion.hpp"

#include <l2cpp/services/Database.hpp>

// Third-party includes
#include <spdlog/spdlog.h>

auto Orm::fetchCharacterPreviews(AccountId const accountId) -> std::vector<Ref<Character>>
try
{
    SQLite::Statement query(Database::instance(), R"(
        SELECT
            name
          , starting_profession
          , sex
          , hair_style
          , hair_color
          , face
          , pos_x
          , pos_y
          , pos_z
          , current_profession
          , xp
          , sp
          , selected
        FROM
            characters
        JOIN
            character_professions AS professions
            ON  professions.character_id = characters.id
            AND professions.profession   = characters.current_profession
        JOIN
            character_previews previews ON previews.character_id = characters.id
        WHERE
            id IN (SELECT character_id FROM character_owners WHERE account_id = ?)
        ORDER BY
            creation_date
    )");
    query.bind(1, accountId);

    std::vector<Ref<Character>> previews;
    while (query.executeStep())
    {
        auto & c = previews.emplace_back(World::addCharacterPreview(accountId)).get();
        c.setName(Utils::toWideString(query.getColumn("name").getString()));
        c.appearance().setStartingProfession(static_cast<Profession>(query.getColumn("starting_profession").getUInt()));
        c.appearance().setSex               (static_cast<Sex>(query.getColumn("sex").getUInt()));
        c.appearance().setHairStyle         (query.getColumn("hair_style").getUInt());
        c.appearance().setHairColor         (query.getColumn("hair_color").getUInt());
        c.appearance().setFace              (query.getColumn("face"      ).getUInt());

        c.setPosX(query.getColumn("pos_x").getInt());
        c.setPosY(query.getColumn("pos_y").getInt());
        c.setPosZ(query.getColumn("pos_z").getInt());

        c.setProfession(static_cast<Profession>(query.getColumn("current_profession").getUInt()));
        c.status().setXp(query.getColumn("xp").getUInt());
        c.status().setSp(query.getColumn("sp").getUInt());

        auto & data = c.addComponent<CharacterSelectionData>();
        data.selected = query.getColumn("selected").getUInt();
    }
    return previews;
}
catch (SQLite::Exception const & e)
{
    SPDLOG_ERROR("Failed to fetch characters for account ID '{}': SQL error '{}' (code: {})",
                  accountId, e.what(), e.getErrorCode());
    return {};
}

void Orm::createCharacter(AccountId const accountId, Character const & c) try
{
    SQLite::Transaction tr(Database::instance());

    SQLite::Statement query(Database::instance(), R"(
        INSERT INTO characters (
            name
          , starting_profession
          , sex
          , hair_style
          , hair_color
          , face
          , pos_x
          , pos_y
          , pos_z
          , current_profession
        ) VALUES (
            :name
          , :starting_profession
          , :sex
          , :hair_style
          , :hair_color
          , :face
          , :pos_x
          , :pos_y
          , :pos_z
          , :current_profession
        )
    )");
    query.bind(":name",                Utils::toString(c.name()));
    query.bind(":starting_profession", std::to_underlying(c.appearance().startingProfession()));
    query.bind(":sex",                 std::to_underlying(c.appearance().sex()));
    query.bind(":hair_style",          c.appearance().hairStyle());
    query.bind(":hair_color",          c.appearance().hairColor());
    query.bind(":face",                c.appearance().face());
    query.bind(":pos_x",               c.position().x);
    query.bind(":pos_y",               c.position().y);
    query.bind(":pos_z",               c.position().z);
    query.bind(":current_profession",  std::to_underlying(c.profession()));
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to insert a new character");

    auto const charId = Database::instance().getLastInsertRowid();

    query = SQLite::Statement(Database::instance(), R"(
        INSERT INTO character_owners VALUES (:account_id, :character_id)
    )");
    query.bind(":account_id",   accountId);
    query.bind(":character_id", charId);
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to insert character owner");

    query = SQLite::Statement(Database::instance(), R"(
        UPDATE
            character_previews
        SET
            selected = FALSE
        WHERE
            character_id IN (SELECT character_id FROM character_owners WHERE account_id = :account_id)
    )");
    query.bind(":account_id", accountId);
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to unselect other characters");

    query = SQLite::Statement(Database::instance(), R"(
        INSERT INTO character_previews (character_id) VALUES (:character_id)
    )");
    query.bind(":character_id", charId);
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to insert character preview");

    tr.commit();
}
catch (SQLite::Exception const & e)
{
    L2CPP_THROW(e.getErrorCode(), "SQL error '{}'", e.what());
}

void Orm::selectCharacter(AccountId const accountId, std::wstring_view const selectedCharName)
try
{
    SQLite::Transaction tr(Database::instance());

    SQLite::Statement query(Database::instance(), R"(
        UPDATE
            character_previews
        SET
            selected = FALSE
        WHERE
            character_id IN (SELECT character_id FROM character_owners WHERE account_id = ?)
    )");
    query.bind(1, accountId);
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to unselect all characters");

    query = SQLite::Statement(Database::instance(), R"(
        UPDATE
            character_previews
        SET
            selected = TRUE
        WHERE
            character_id = (SELECT id FROM characters WHERE name LIKE ? LIMIT 1)
    )");
    query.bind(1, Utils::toString(selectedCharName));
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to select character");

    tr.commit();
}
catch (SQLite::Exception const & e)
{
    L2CPP_THROW(e.getErrorCode(), "SQL error '{}'", e.what());
}
