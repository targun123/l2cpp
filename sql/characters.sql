CREATE TABLE IF NOT EXISTS characters (
    id                  INTEGER          PRIMARY KEY,
    name                TEXT    NOT NULL UNIQUE,
    starting_profession INTEGER NOT NULL,
    sex                 INTEGER NOT NULL,
    hair_style          INTEGER NOT NULL,
    hair_color          INTEGER NOT NULL,
    face                INTEGER NOT NULL,
    title               TEXT    NOT NULL DEFAULT ''
);
