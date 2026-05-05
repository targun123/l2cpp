/// @author    Chnossos
/// @date      Created on 2026-05-05

#include "Database.hpp"

// Project includes

void Database::init()
{
    auto & db = instance();

    db.exec(R"(CREATE TABLE IF NOT EXISTS accounts (
        id            INTEGER          PRIMARY KEY,
        name          TEXT    NOT NULL UNIQUE,
        password      TEXT    NOT NULL,
        creation_date TEXT    NOT NULL DEFAULT DATETIME
    );
    INSERT OR IGNORE INTO accounts (name, password) VALUES ("Admin", "a");
    )");

    db.exec(R"(CREATE TABLE IF NOT EXISTS characters (
        id         INTEGER          PRIMARY KEY,
        name       TEXT    NOT NULL UNIQUE,
        race       INTEGER NOT NULL,
        sex        INTEGER NOT NULL,
        hair_style INTEGER NOT NULL,
        hair_color INTEGER NOT NULL,
        face       INTEGER NOT NULL
    );)");

    db.exec(R"(CREATE TABLE IF NOT EXISTS character_owners (
        account_id   INTEGER,
        character_id INTEGER,
        PRIMARY KEY (account_id, character_id),
        FOREIGN KEY (account_id)   REFERENCES accounts   (id) ON DELETE CASCADE,
        FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
    );)");

    db.exec(R"(CREATE TABLE IF NOT EXISTS character_previews (
        character_id        INTEGER,
        selected            INTEGER NOT NULL DEFAULT TRUE,
        deletion_time_point INTEGER NOT NULL DEFAULT 0,
        FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
    );)");
}

auto Database::instance() -> SQLite::Database &
{
    static SQLite::Database db("data/database", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    return db;
}
