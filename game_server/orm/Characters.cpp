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
#include "../game/ui/Shortcut.hpp"
#include "../utils/Conversion.hpp"

#include <l2cpp/services/Database.hpp>

// Third-party includes
#include <spdlog/spdlog.h>

namespace
{
    void saveStatus(u32 characterId, Character const & c);

    void saveInventory(u32 characterId, Character const & c);
    void loadInventory(u32 characterId, Character & c);

    void saveShortcuts(u32 characterId, Character const & c);
    void loadShortcuts(u32 characterId, Character & c);
}

void Orm::saveCharacter(Character const & c)
{
    SQLite::Statement idQuery{Database::instance(), R"(
        SELECT id FROM characters WHERE name = :name LIMIT 1
    )"};
    idQuery.bind(":name", Utils::toString(c.name()));
    L2CPP_B_ASSERT(idQuery.executeStep(), "Failed to retrieve character id");
    auto const id = idQuery.getColumn("id").getUInt();

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
            id = :character_id
    )");
    query.bind(":character_id", id);
    query.bind(":title",        Utils::toString(c.title()));
    query.bind(":pos_x",        c.position().x);
    query.bind(":pos_y",        c.position().y);
    query.bind(":pos_z",        c.position().z);
    query.bind(":orientation",  c.position().orientation);
    L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to save character");

    saveStatus   (id, c);
    // saveInventory(id, c);
    saveShortcuts(id, c);
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

    auto const id = query.getColumn("id").getUInt();
    loadInventory(id, c);
    loadShortcuts(id, c);
}

namespace
{
    void saveStatus(u32 const characterId, Character const & c)
    {
        auto query = SQLite::Statement(Database::instance(), R"(
            UPDATE
                character_statuses
            SET
                xp = :xp
              , sp = :sp
              , hp = :hp
              , mp = :mp
              , cp = :cp
            WHERE
                character_id   = :character_id
                AND profession = :profession
        )");
        query.bind(":character_id", characterId);
        query.bind(":xp",           c.status().xp());
        query.bind(":sp",           c.status().sp());
        query.bind(":hp",           static_cast<u32>(c.stats()[StatId::CurHp]));
        query.bind(":mp",           static_cast<u32>(c.stats()[StatId::CurMp]));
        query.bind(":cp",           static_cast<u32>(c.stats()[StatId::CurCp]));
        query.bind(":profession",   std::to_underlying(c.profession()));
        L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to save character's profession");
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

            Item item{query.getColumn("id").getInt64()};
            item.tmplate      = std::move(*itemTemplate);
            item.enchantLevel = enchantLevel.isNull() ? 0 : static_cast<u8>(enchantLevel.getUInt());
            item.quantity     = quantity    .isNull() ? 1 : quantity.getUInt();

            inventory.add(std::move(item));
        }
    }

    void saveShortcuts(u32 const characterId, Character const & c)
    {
        auto const & shortcuts = c.shortcuts();

        static std::vector<Ref<Shortcut const>> nonEmptyShortcuts;
        nonEmptyShortcuts.reserve(shortcuts.size());
        nonEmptyShortcuts.clear();

        std::vector<size_t> indexes;
        for (auto const & s : shortcuts)
        {
            if (!s.isEmpty())
            {
                nonEmptyShortcuts.emplace_back(s);
                indexes.emplace_back(*s.index());
            }
        }

        std::string queryText = R"(
            DELETE FROM
                character_shortcuts
            WHERE
                character_id   = :character_id
                AND profession = :profession
        )";

        if (!nonEmptyShortcuts.empty())
            queryText += std::format(R"( AND "index" NOT IN ({:n}))", indexes);

        SQLite::Transaction tr{Database::instance()};

        auto query = SQLite::Statement{Database::instance(), queryText };
        query.bind(":character_id", characterId);
        query.bind(":profession",   std::to_underlying(c.profession()));
        L2CPP_F_ASSERT([&] { query.exec(); }, "Failed to remove emptied shortcuts");

        query = SQLite::Statement{Database::instance(), R"(
            INSERT OR REPLACE INTO character_shortcuts
                ( character_id,  profession, "index",  type,  target_id,  extra_info)
            VALUES
                (:character_id, :profession, :index, :type, :target_id, :extra_info)
        )"};

        for (Shortcut const & s : nonEmptyShortcuts)
        {
            query.reset();
            query.bind(":character_id", characterId);
            query.bind(":profession",   std::to_underlying(c.profession()));
            query.bind(":index",        static_cast<u32>(*s.index()));
            query.bind(":type",         std::to_underlying(s.type()));
            query.bind(":target_id",    s.targetId());
            if (s.type() == ShortcutType::Skill)
                query.bind(":extra_info", s.skillLevel());
            else
                query.bind(":extra_info");

            query.exec();
        }

        tr.commit();
    }

    void loadShortcuts(u32 const characterId, Character & c)
    {
        SQLite::Statement query{Database::instance(), std::format(R"(
            SELECT
                *
            FROM
                character_shortcuts
            WHERE
                character_id   = :character_id
                AND profession = :profession
        )")};
        query.bind(":character_id", characterId);
        query.bind(":profession",   std::to_underlying(c.profession()));

        while (query.executeStep())
        {
            Shortcut s;
            s.setIndex(query.getColumn("index").getUInt());
            s.setType(static_cast<ShortcutType>(query.getColumn("type").getUInt()));
            s.setTargetId(query.getColumn("target_id").getUInt());

            if (s.type() == ShortcutType::Skill)
                s.setSkillLevel(static_cast<SkillLevel>(query.getColumn("extra_info").getUInt()));

            c.setShortcut(std::move(s));
        }
    }
}
