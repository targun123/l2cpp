CREATE TRIGGER IF NOT EXISTS starting_items
    AFTER
        INSERT ON characters
    BEGIN
        INSERT INTO items (template_id, owner_id, storage_id, quantity, equipped) VALUES
                          (6408,        NEW.id,   0,          NULL,     TRUE),  -- Formal Wear
                          (6621,        NEW.id,   0,          NULL,     FALSE), -- Infinity Spear
                          (6611,        NEW.id,   0,          NULL,     TRUE),  -- Infinity Blade
                          (6377,        NEW.id,   0,          NULL,     TRUE),  -- Imperial Crusader Shield
                          (6618,        NEW.id,   0,          NULL,     FALSE), -- Infinity Fangs
                          (6619,        NEW.id,   0,          NULL,     FALSE), -- Infinity Bow
                          (1345,        NEW.id,   0,          100,      FALSE); -- Arrow (S-grade)
    END;
