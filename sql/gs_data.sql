INSERT OR IGNORE INTO characters (
    name
  , starting_profession
  , sex
  , hair_style
  , hair_color
  , face
  , title
  , pos_x
  , pos_y
  , pos_z
  , current_profession
)
VALUES
('Admin',  0, 0, 0, 0, 0, '{l2cpp}', -83968, 244634, -3500, 88),
('Admin2', 1, 1, 0, 0, 0, '{l2cpp}', -83968, 244634, -3500, 25);

INSERT OR IGNORE INTO character_professions VALUES (1, 88, 2099275833, 0);
INSERT OR IGNORE INTO character_professions VALUES (2, 25,          0, 0);

INSERT OR IGNORE INTO character_owners VALUES (1, 1);
INSERT OR IGNORE INTO character_owners VALUES (2, 2);

INSERT OR IGNORE INTO character_previews (character_id) VALUES (1);
INSERT OR IGNORE INTO character_previews (character_id) VALUES (2);
