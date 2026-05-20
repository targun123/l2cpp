INSERT OR IGNORE INTO characters (
    account_id
  , name
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
(1, 'Admin',  0,  0, 0, 0, 0, '{l2cpp}', -83968, 244634, -3500, 88), -- on subclass Duelist
(2, 'Admin2', 25, 1, 0, 0, 0, '{l2cpp}', -83968, 244634, -3500, 25); -- on main class Elven Mystic

INSERT OR IGNORE INTO character_professions VALUES
(1,  0), -- Human Fighter (main class)
(1, 88), -- Duelist       (1st subclass)
(2, 25); -- Elven Mystic  (main class)

INSERT OR IGNORE INTO character_statuses VALUES
(1,  0,          0, 0, 1, 1, 1), -- Human Figter lv.  1    0xp 0sp 1hp 1mp 1cp
(1, 88, 2099275833, 0, 1, 1, 1), -- Duelist      lv. 78 100%xp 0sp 1hp 1mp 1cp
(2, 25,          0, 0, 1, 1, 1); -- Elven Mystic lv.  1    0xp 0sp 1hp 1mp 1cp

INSERT OR IGNORE INTO npc_htmls (
    template_id
  , page
  , html_text
)
VALUES
(7123, 0, '<html><body>Guard Vesa:<br>If you cross <font color="LEVEL">Death Pass</font> in the north, you can get to <font color="LEVEL">Oren.</font> I am originally from there, but because of the war with Elmore, they are severely restricting passage for the common people. I worry about whether my family is safe.<br><a action="bypass npc_%objectId%_Chat 1">Ask about Death Pass.</a><br><a action="bypass npc_%objectId%_Chat 2">Ask about Oren.</a><br><a action="bypass npc_%objectId%_TerritoryStatus">View the information about the lord and the tax rate of the manor.</a><br><a action="bypass npc_%objectId%_Quest">Quest</a></body></html>'); -- Guard Vesa (Main Page)

INSERT OR IGNORE INTO npc_htmls (template_id, page, html_text)
VALUES (7123, 1, '<html><body>Guard Vesa:<br>I have never been to the Corridor of Death. I heard it is a long and dangerous road, because of the monsters that come out from the Dragon Valley. If you want to know about the Corridor of Death, you should ask my partner, <font color="LEVEL">Zerome</font>. He recently returned from making a patrol of that place.<br><a action="bypass npc_%objectId%_Chat 0">Return</a></body></html>');

INSERT OR IGNORE INTO npc_htmls (template_id, page, html_text)
VALUES (7123, 2, '<html><body>Guard Vesa:<br>The region of Oren is north of Giran. I once had a chance to meet their guards, and they seem to have a very strict military discipline. Isn''t it really ironic that the Ivory Tower, the traditional school of magic, is there?<br><a action="bypass npc_%objectId%_Chat 0">Return</a></body></html>');

INSERT OR IGNORE INTO npc_htmls (
    template_id
  , page
  , html_text
)
VALUES
(7009, 0, '<html><body>Newbie Helper:<br>Welcome to Cedric''s Training Hall. I will be teaching you the basics of combat.<br>Please click on <font color="LEVEL">Quest</font>, in your Chat window.<br><a action="bypass npc_%objectId%_Quest">Quest</a></body></html>'); -- Newbie Helper (Main Page)
