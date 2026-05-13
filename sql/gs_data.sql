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
('Admin',  0, 0, 0, 0, 0, '{l2cpp}', -83968, 244634, -3500, 88), -- on subclass Duelist
('Admin2', 1, 1, 0, 0, 0, '{l2cpp}', -83968, 244634, -3500, 25); -- on main class Elven Mystic

INSERT OR IGNORE INTO character_owners VALUES (1, 1), (2, 2);

INSERT OR IGNORE INTO character_professions VALUES
(1,  0), -- Human Fighter (main class)
(1, 88), -- Duelist       (1st subclass)
(2, 25); -- Elven Mystic  (main class)

INSERT OR IGNORE INTO character_statuses VALUES
(1,  0,          0, 0, 1, 1, 1), -- Human Figter lv.  1    0xp 0sp 1hp 1mp 1cp
(1, 88, 2099275833, 0, 1, 1, 1), -- Duelist      lv. 78 100%xp 0sp 1hp 1mp 1cp
(2, 25,          0, 0, 1, 1, 1); -- Elven Mystic lv.  1    0xp 0sp 1hp 1mp 1cp
