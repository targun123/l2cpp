CREATE TABLE IF NOT EXISTS character_shortcuts (
    character_id INTEGER NOT NULL
  , profession   INTEGER NOT NULL
  , "index"      INTEGER NOT NULL
  , type         INTEGER NOT NULL
  , target_id    INTEGER NOT NULL
  , extra_info   INTEGER
  , PRIMARY KEY (character_id, profession, "index")
  , FOREIGN KEY (character_id, profession) REFERENCES character_professions (character_id, profession) ON DELETE CASCADE
);
