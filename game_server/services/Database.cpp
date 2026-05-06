/// @author    Chnossos
/// @date      Created on 2026-05-05

#include "Database.hpp"

// Project includes

void Database::init()
{
    auto & db = instance();

    //db.exec(R"(DROP TABLE IF EXISTS accounts; CREATE TABLE IF NOT EXISTS accounts (
    db.exec(R"(CREATE TABLE IF NOT EXISTS accounts (
        id            INTEGER          PRIMARY KEY,
        name          TEXT    NOT NULL UNIQUE,
        password      BLOB    NOT NULL,
        creation_date TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP
    );
    INSERT OR IGNORE INTO accounts (name, password) VALUES ("Admin", "a");
    INSERT OR IGNORE INTO accounts (name, password) VALUES ("Admin2", "a");
    )");

    //db.exec(R"(DROP TABLE IF EXISTS characters; CREATE TABLE IF NOT EXISTS characters (
    db.exec(R"(CREATE TABLE IF NOT EXISTS characters (
        id         INTEGER          PRIMARY KEY,
        name       TEXT    NOT NULL UNIQUE,
        race       INTEGER NOT NULL,
        sex        INTEGER NOT NULL,
        hair_style INTEGER NOT NULL,
        hair_color INTEGER NOT NULL,
        face       INTEGER NOT NULL
    );
    INSERT OR IGNORE INTO characters (name, race, sex, hair_style, hair_color, face) VALUES ("Admin",  0, 0, 0, 0, 0);
    INSERT OR IGNORE INTO characters (name, race, sex, hair_style, hair_color, face) VALUES ("Admin2", 1, 1, 0, 0, 0);
    )");

    //db.exec(R"(DROP TABLE IF EXISTS character_owners; CREATE TABLE IF NOT EXISTS character_owners (
    db.exec(R"(CREATE TABLE IF NOT EXISTS character_owners (
        account_id   INTEGER,
        character_id INTEGER,
        PRIMARY KEY (account_id, character_id),
        FOREIGN KEY (account_id)   REFERENCES accounts   (id) ON DELETE CASCADE,
        FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
    );
    INSERT OR IGNORE INTO character_owners VALUES (1, 1);
    INSERT OR IGNORE INTO character_owners VALUES (2, 2);
    )");

    //db.exec(R"(DROP TABLE IF EXISTS character_previews; CREATE TABLE IF NOT EXISTS character_previews (
    db.exec(R"(CREATE TABLE IF NOT EXISTS character_previews (
        character_id  INTEGER UNIQUE,
        selected      INTEGER NOT NULL DEFAULT TRUE,
        creation_date TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
    );
    INSERT OR IGNORE INTO character_previews (character_id) VALUES (1);
    INSERT OR IGNORE INTO character_previews (character_id) VALUES (2);
    )");
}

auto Database::instance() -> SQLite::Database &
{
    static SQLite::Database db("data/database", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    return db;
}
