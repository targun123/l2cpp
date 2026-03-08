# l2cpp

[Lineage 2, Chronicle 4][4] C++23 emulation servers.

This is my latest passion project. I've been wanting to do this since I was 15, and here I am 16 years later with enough programming skills to do this on my own.

Even though I never played C4 (began on Interlude), I'm targeting this one because the game is less big and the protocol is slightly easier. I'd love to support more protocols in the future.

Last update: `2026-03-08`.

## The goal

Go fast (but not too fast) and break things. YAGNI. Do solo gameplay for now, broadcast later. Implement game design patterns I've studied years ago. Don't be too clever but have fun implementing and testing. DO **NOT** USE MULTIPLE THREADS.

## What's available?

Unless explicitely specified, don't assume anything to be available. Here's what's been done so far:

**Core**
 - Packet creation, reading and sending;
 - Auth & Game protocols encryption/decryption;
 - Exception stack handling.

**Login Server**
 - Only one user is served at any point in time;
 - Users can log in with any credentials, no data is stored on disk (yet);
 - Server list always displays 2 server (one up, one down), both lead to game server if started.

**Game Server**
 - Protocol 656 only; 
 - Character creation (but not restored as no data is stored on disk yet);
 - One male human fighter named "test" is automatically created (for uh… test purposes);
 - World entering (every character spawns at Talking Island);
 - Inventory list (with one weapon that can be equipped or unequipped);
 - Mini-map;
 - Character status;
 - Skills list (with only GM-only _Super Haste lv. 4_ 😎);
 - Skill use (only _Super Haste_ is supported; no animation supported yet);
 - Shortcuts;
 - Target select and unselect;
 - Chat (no restriction).

## What's coming next

 - There's much internal work I want to do around the structure of the code itself: game loop, ECS, serialization…
 - Equip/Unequip any wearable item;
 - Load all skills from disk and look them up through GM UI;
 - Implement some basic skill logic (buff, physical/magical offensive on non-self target).

## How to build

I develop with the following environment:
 - Windows 11
 - Visual Studio Build Tools 2026
 - CMake 4.2
 - Conan (latest as of 2026-03-08)
 - Ninja (latest as of 2026-03-08)

```shell
git clone -b dev https://github.com/Chnossos/l2cpp.git && cd l2cpp
conan install -b missing -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
cmake --preset conan-default
cmake --build build --preset conan-release # or conan-debug
```

## Credits

[l2auth][1] (by [@Ruk33][2]) and [L2J-L2jMobius][3].

[1]: https://github.com/Ruk33/l2auth
[2]: https://github.com/Ruk33
[3]: https://gitlab.com/MobiusDevelopment/L2J_Mobius/
[4]: https://legacy-lineage2.com/news/chronicle4_01.html
