CREATE TABLE IF NOT EXISTS character_previews (
    character_id  INTEGER UNIQUE,
    selected      INTEGER NOT NULL DEFAULT TRUE,
    creation_date TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
);
