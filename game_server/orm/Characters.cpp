/// @author    Chnossos
/// @date      Created on 2026-05-09

#include "Characters.hpp"

// Project includes
#include "../game/actor/Character.hpp"
#include "../game/components/CharacterStatus.hpp"
#include "../game/components/Position.hpp"
#include "../game/components/Stats.hpp"
#include "../game/inventory/Item.hpp"
#include "../game/inventory/ItemStorage.hpp"
#include "../game/inventory/ItemTemplateDirectory.hpp"
#include "../utils/Conversion.hpp"

#include <l2cpp/services/Database.hpp>

// Third-party includes
#include <spdlog/spdlog.h>

static void loadInventory(u32 characterId, Character & c);

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
            name = :name
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
            character_statuses
        SET
            xp = :xp
          , sp = :sp
          , hp = :hp
          , mp = :mp
          , cp = :cp
        WHERE
            character_id = (SELECT id FROM characters WHERE name LIKE :name LIMIT 1)
            AND profession = :profession
    )");
    query.bind(":xp",         c.status().xp());
    query.bind(":sp",         c.status().sp());
    query.bind(":hp",         static_cast<u32>(c.stats()[StatId::CurHp]));
    query.bind(":mp",         static_cast<u32>(c.stats()[StatId::CurMp]));
    query.bind(":cp",         static_cast<u32>(c.stats()[StatId::CurCp]));
    query.bind(":name",       Utils::toString(c.name()));
    query.bind(":profession", std::to_underlying(c.profession()));
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to save character's profession");
}

void Orm::loadCharacter(Character & c)
{
    // Here we do not load the preview fields again, we load the rest in order to enter the World
    SQLite::Statement query(Database::instance(), R"(
        SELECT
            id
          , title
          , orientation
        FROM
            characters
        WHERE
            name = :name
        LIMIT
            1
    )");
    query.bind(":name", Utils::toString(c.name()));
    L2CPP_B_ASSERT(query.executeStep(), "Failed to load character");

    c.setTitle(Utils::toWideString(query.getColumn("title").getString()));
    c.setOrientation(query.getColumn("orientation").getInt());

    loadInventory(query.getColumn("id").getUInt(), c);
}

void loadInventory(u32 const characterId, Character & c)
{
    SQLite::Statement query{Database::instance(), R"(
        SELECT
            template_id
          , quantity
          , enchant_level
        FROM
            items
        WHERE
            owner_id = :character_id AND storage_id = 0 AND equipped = FALSE
    )"};
    query.bind(":character_id", characterId);

    auto & inventory = c.inventory();
    while (query.executeStep())
    {
        auto const templateId   = query.getColumn("template_id").getUInt();
        auto const itemTemplate = ItemTemplateDirectory::find(templateId);
        if (!itemTemplate)
        {
            SPDLOG_ERROR("ItemTemplate id '{}' not loaded but a character requests it", templateId);
            continue;
        }

        auto const enchantLevel = query.getColumn("enchant_level");
        auto const quantity     = query.getColumn("quantity");

        Item item;
        item.tmplate      = std::move(*itemTemplate);
        item.enchantLevel = enchantLevel.isNull() ? 0 : static_cast<u8>(enchantLevel.getUInt());
        item.quantity     = quantity    .isNull() ? 1 : quantity.getUInt();

        inventory.add(std::move(item));
    }
}
