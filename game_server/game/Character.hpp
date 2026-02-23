/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <string>

struct Character
{
    u32 id = 0;
    std::wstring name;
    u32 clanId = 0;
    u32 sex = 0;
    u32 raceId = 0;
    u32 active = 1;
    double currentHp = 500;
    double currentMp = 500;
    u32 sp = 0;
    u32 xp = 0;
    u32 level = 1;
    u32 karma = 0;
    u32 pkCount = 0;
    u32 pvpCount = 0;
    u32 hairStyleId = 0;
    u32 hairColorId = 0;
    u32 faceId = 0;
    double maxHp = 500;
    double maxMp = 500;
    u32 deleteTime = 0;
    u32 classId = 0;
    u32 selected = 0;
    u8 enchantEffect = 0;
    u32 augmentationId = 0;
    u32 transformationId = 0;

    int posX = -83968;
    int posY = 244634;
    int posZ = -3000;
};

/*
slots.add(Inventory.PAPERDOLL_UNDER);
            slots.add(Inventory.PAPERDOLL_REAR);
            slots.add(Inventory.PAPERDOLL_LEAR);
            slots.add(Inventory.PAPERDOLL_NECK);
            slots.add(Inventory.PAPERDOLL_RFINGER);
            slots.add(Inventory.PAPERDOLL_LFINGER);
        slots.add(Inventory.PAPERDOLL_HEAD);
        slots.add(Inventory.PAPERDOLL_RHAND);
        slots.add(Inventory.PAPERDOLL_LHAND);
        slots.add(Inventory.PAPERDOLL_GLOVES);
        slots.add(Inventory.PAPERDOLL_CHEST);
        slots.add(Inventory.PAPERDOLL_LEGS);
        slots.add(Inventory.PAPERDOLL_FEET);
        slots.add(Inventory.PAPERDOLL_BACK);
        slots.add(Inventory.PAPERDOLL_LRHAND);
        slots.add(Inventory.PAPERDOLL_HAIR);
        slots.add(Inventory.PAPERDOLL_HAIR2);
        slots.add(Inventory.PAPERDOLL_RBRACELET);
        slots.add(Inventory.PAPERDOLL_LBRACELET);
        slots.add(Inventory.PAPERDOLL_DECO1);
        slots.add(Inventory.PAPERDOLL_DECO2);
        slots.add(Inventory.PAPERDOLL_DECO3);
        slots.add(Inventory.PAPERDOLL_DECO4);
        slots.add(Inventory.PAPERDOLL_DECO5);
        slots.add(Inventory.PAPERDOLL_DECO6);
*/

/*
 *			// <<<<< The next block is repeated as many times as the spells on the acca.
XX XX XX XX XX 00 00	// string containing a nickname of the spell
XX XX XX XX		// ID of the war
XX XX XX XX XX 00 00	// string with login
XX XX XX XX		// ID of the war
XX XX XX XX		// SessionID
XX XX XX XX		// ID clan
00 00 00		/ unknown
XX XX XX XX		// floor (0x00 - male, 0х01 - woman
XX XX XX XX		// Rassa
XX XX XX XX		// class
XX XX XX XX		// active. (?)
XX XX XX XX		// X - not used
XX XX XX XX		// Y - not used
XX XX XX XX		// Z - not used
XX XX XX XX XX XX XX XX XX XX	// current HP (number stored in double format)
XX XX XX XX XX XX XX XX XX XX	// current MP (number is stored in double format)
XX XX XX XX		// SP
XX XX XX XX XX XX XX XX XX XX	// EXP (type __int64)
XX XX XX XX		// level
XX XX XX XX		// Karma
XX XX XX XX...		// array of 36 zeros, destination is unknown
XX XX XX XX		ItemObjectID Under
XX XX XX XX		ItemObjectID REar
XX XX XX XX		ItemObjectID LEAR
XX XX XX XX		ItemObjectID Neck
XX XX XX XX		ItemObjectID RFinger
XX XX XX XX		/ ItemObjectID LFinger
XX XX XX XX		ItemObjectID Head
XX XX XX XX		ItemObjectID RHand
XX XX XX XX		ItemObjectID LHand
XX XX XX XX		ItemObjectID Gloves
XX XX XX XX		ItemObjectID Chest
XX XX XX XX		ItemObjectID Legs
XX XX XX XX		/ ItemObjectID Feet
XX XX XX XX		// ItemObjectID Back
XX XX XX XX		ItemObjectID ЛRHand
XX XX XX XX		ItemObjectID Hair
XX XX XX XX		// ItemID Under
XX XX XX XX		// ItemID REar
XX XX XX XX		/ ItemID LEAR
XX XX XX XX		// ItemID Neck
XX XX XX XX		// ItemID RFinger
XX XX XX XX		/ ItemID LFinger
XX XX XX XX		/ ItemID Head
XX XX XX XX		/ ItemID RHand
XX XX XX XX		// ItemID LHand
XX XX XX XX		ItemID Gloves
XX XX XX XX		// ItemID Chest
XX XX XX XX		// ItemID Legs
XX XX XX XX		ItemID Feet
XX XX XX XX		/ ItemID Back
XX XX XX XX		ItemID LRHand
XX XX XX XX		ItemID Hair
XX XX XX XX		// hairstyle
XX XX XX XX		// Hair color
XX XX XX XX		// Type of face
XX XX XX XX XX XX XX XX XX XX	// maximum HP (number stored in double format)
XX XX XX XX XX XX XX XX XX XX	// maximum MP (the number is stored in double format)
XX XX XX XX		// time before the passage of the spell in seconds. If 0, then the spell is not put on the removal
XX XX XX XX		Class ID(basic ?)
XX XX XX XX		// used by the client to determine the spell which were last visited, he has 1, the rest - 0
XX			// level of sharpening gun
XX XX XX XX		// augmentation id
*/
