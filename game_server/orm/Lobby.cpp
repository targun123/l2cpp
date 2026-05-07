/// @author    Chnossos
/// @date      Created on 2026-05-06

#include "Lobby.hpp"

// Project includes
#include "../game/actor/Character.hpp"
#include "../game/components/CharacterSelectionData.hpp"
#include "../game/components/PlayerAppearance.hpp"
#include "../game/lobby/CharacterCreationParameters.hpp"
#include "../utils/Conversion.hpp"

#include <l2cpp/services/Database.hpp>

// Third-party includes
#include <spdlog/spdlog.h>

auto Orm::fetchCharacterPreviews(AccountId const accountId) -> std::vector<std::unique_ptr<Character>>
try
{
    static SQLite::Statement query(Database::instance(), R"(
        SELECT name, race, sex, hair_style, hair_color, face, selected FROM characters
        JOIN character_previews cp ON cp.character_id = characters.id
        WHERE id IN (SELECT character_id FROM character_owners WHERE account_id = ?)
        ORDER BY creation_date
    )");

    query.reset();
    query.bind(1, accountId);

    std::vector<std::unique_ptr<Character>> previews;
    while (query.executeStep())
    {
        auto const & c = previews.emplace_back(std::make_unique<Character>());
        c->setName(Utils::toWideString(query.getColumn("name").getString()));
        c->appearance().race        = static_cast<Race>(query.getColumn("race").getUInt());
        c->appearance().sex         = static_cast<Sex >(query.getColumn("sex" ).getUInt());
        c->appearance().hairStyleId = query.getColumn("hair_style").getUInt();
        c->appearance().hairColorId = query.getColumn("hair_color").getUInt();
        c->appearance().faceId      = query.getColumn("face"      ).getUInt();

        auto & data = c->addComponent<CharacterSelectionData>();
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

void Orm::createCharacter(AccountId const accountId, CharacterCreationParameters const & params)

try
{
    SQLite::Transaction tr(Database::instance());

    SQLite::Statement charQuery(Database::instance(), R"(
        INSERT INTO characters (name, race, sex, hair_style, hair_color, face)
            VALUES (:name, :race, :sex, :hair_style, :hair_color, :face)
    )");
    charQuery.bind(":name",       Utils::toString(params.name));
    charQuery.bind(":race",       std::to_underlying(params.race));
    charQuery.bind(":sex",        std::to_underlying(params.sex));
    charQuery.bind(":hair_style", params.hairStyle);
    charQuery.bind(":hair_color", params.hairColor);
    charQuery.bind(":face",       params.face);
    charQuery.exec();

    auto const charId = Database::instance().getLastInsertRowid();

    SQLite::Statement charOwnerQuery(Database::instance(),
                                     R"(INSERT INTO character_owners VALUES (:account_id, :character_id))");
    charOwnerQuery.bind(":account_id",   accountId);
    charOwnerQuery.bind(":character_id", charId);
    charOwnerQuery.exec();

    SQLite::Statement unselectCharPreviews(Database::instance(), R"(
        UPDATE character_previews SET selected = FALSE WHERE character_id = (
            SELECT character_id FROM character_owners WHERE account_id = :account_id)
    )");
    unselectCharPreviews.bind(":account_id", accountId);
    unselectCharPreviews.exec();

    SQLite::Statement charPreviewQuery(Database::instance(),
                                       R"(INSERT INTO character_previews (character_id) VALUES (:character_id))");
    charPreviewQuery.bind(":character_id", charId);
    charPreviewQuery.exec();

    tr.commit();
}
catch (SQLite::Exception const & e)
{
    L2CPP_THROW(e.getErrorCode(), "SQL error '{}'", e.what());
}
