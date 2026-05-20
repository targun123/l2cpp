CREATE TABLE IF NOT EXISTS npc_htmls (
    id              INTEGER PRIMARY KEY AUTOINCREMENT
  , template_id     INTEGER NOT NULL
  , page            INTEGER NOT NULL          -- 0 is the main page, 1+ are sub-chats (Chat 1, Chat 2 etc.)
  , html_text       TEXT NOT NULL             -- full html raw string content
);