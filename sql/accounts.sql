CREATE TABLE IF NOT EXISTS accounts (
    id            INTEGER PRIMARY KEY,
    name          TEXT    NOT NULL UNIQUE,
    password      BLOB    NOT NULL,
    creation_date TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP
);
