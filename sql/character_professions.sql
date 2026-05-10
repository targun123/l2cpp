CREATE TABLE IF NOT EXISTS character_professions (
    character_id INTEGER
  , profession   INTEGER NOT NULL
  , xp           INTEGER NOT NULL DEFAULT 0
  , sp           INTEGER NOT NULL DEFAULT 0
  , PRIMARY KEY (character_id, profession)
  , FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
);
