CREATE TABLE IF NOT EXISTS characters (
    id                  INTEGER PRIMARY KEY
  , account_id          INTEGER NOT NULL
  , name                TEXT    NOT NULL UNIQUE
  , starting_profession INTEGER NOT NULL
  , sex                 INTEGER NOT NULL
  , hair_style          INTEGER NOT NULL
  , hair_color          INTEGER NOT NULL
  , face                INTEGER NOT NULL
  , title               TEXT    NOT NULL DEFAULT ''
  , pos_x               INTEGER NOT NULL
  , pos_y               INTEGER NOT NULL
  , pos_z               INTEGER NOT NULL
  , orientation         INTEGER NOT NULL DEFAULT 0
  , current_profession  INTEGER NOT NULL
  , FOREIGN KEY (account_id) REFERENCES accounts (id) ON DELETE CASCADE
);
