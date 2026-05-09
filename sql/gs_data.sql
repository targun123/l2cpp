INSERT OR IGNORE INTO characters (name, starting_profession, sex, hair_style, hair_color, face, title)
    VALUES ('Admin',  0, 0, 0, 0, 0, '{l2cpp}');
INSERT OR IGNORE INTO characters (name, starting_profession, sex, hair_style, hair_color, face, title)
    VALUES ('Admin2', 0, 1, 0, 0, 0, '{l2cpp}');

INSERT OR IGNORE INTO character_owners VALUES (1, 1);
INSERT OR IGNORE INTO character_owners VALUES (2, 2);

INSERT OR IGNORE INTO character_previews (character_id) VALUES (1);
INSERT OR IGNORE INTO character_previews (character_id) VALUES (2);
