CREATE TABLE IF NOT EXISTS character_owners (
    account_id   INTEGER
  , character_id INTEGER
  , PRIMARY KEY (account_id, character_id)
  , FOREIGN KEY (account_id)   REFERENCES accounts   (id) ON DELETE CASCADE
  , FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
);
