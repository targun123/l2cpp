CREATE TABLE IF NOT EXISTS character_owners (
    account_id   INTEGER
  , character_id INTEGER
  , PRIMARY KEY (account_id, character_id)
  , FOREIGN KEY (account_id)   REFERENCES accounts   (id) ON DELETE CASCADE
  , FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
);

CREATE TRIGGER IF NOT EXISTS auto_insert_character_preview
    AFTER
        INSERT ON character_owners
    BEGIN
        UPDATE character_previews SET selected = FALSE WHERE character_id IN (
            SELECT character_id FROM character_owners WHERE account_id = NEW.account_id
        );
        INSERT INTO character_previews (character_id) VALUES (NEW.character_id);
    END;
