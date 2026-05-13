CREATE TABLE IF NOT EXISTS character_previews (
    character_id  INTEGER UNIQUE,
    selected      INTEGER NOT NULL DEFAULT TRUE,
    creation_date TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (character_id) REFERENCES characters (id) ON DELETE CASCADE
);

CREATE TRIGGER IF NOT EXISTS auto_insert_character_preview
    AFTER
        INSERT ON characters
    BEGIN
        UPDATE character_previews SET selected = FALSE WHERE character_id IN (
            SELECT character_id FROM characters WHERE account_id = NEW.account_id
        );
        INSERT INTO character_previews (character_id) VALUES (NEW.id);
    END;
