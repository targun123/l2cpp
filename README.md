# l2cpp

[Lineage 2, Chronicle 4: Scions of Destiny][4] C++23 emulation servers.

This is my latest passion project. I've been wanting to do this since I was 15, and here I am 16 years later with enough programming skills to do this on my own.

Even though I never played C4 (began on Interlude), I'm targeting this one because the game is less big and the protocol is slightly easier. I'd love to support more protocols in the future.

## The goal

Go fast (but not too fast) and break things. YAGNI. Do solo gameplay for now, broadcast later. Implement game design patterns I've studied years ago. Don't be too clever but have fun implementing and testing. DO **NOT** USE MULTIPLE THREADS.

## What's available?

Unless explicitely specified, don't assume anything to be available. Here's what's been done so far:

**Core**
- [x] Packet creation, reading and sending;
- [x] Auth & Game protocols encryption/decryption;
- [x] Exception stack handling.

**Login Server**
- [x] Protocol `c621` only;
- [x] Only one user is served at any point in time for now;
- [x] Users can log in with any credentials, no data is stored on disk (yet);
- [x] Server list always displays 2 server (one up, one down), both lead to game server.

**Game Server**
- [x] Protocol 656 only; 
- [x] Character creation (but not restored as no data is stored on disk yet);
- [x] One male human fighter named "test" is automatically created (for uh… test purposes);
- [x] World entering (every character spawns at Talking Island);
- [x] Moving around (not broadcasted yet);
- [x] Inventory list (gear items can be equipped and unequipped);
- [x] Mini-map;
- [x] Character status;
- [x] Skills list (can give yourself any skill with `//learn <skill_id> <skill_level>` command);
- [x] Skill use (animation only);
- [x] Shortcuts (skills and items);
- [x] Target select and unselect;
- [x] Chat (no restrictions);
- [x] Can spawn any NPC with `//spawn <npc_id>` (collision is bugged);
- [x] game loop to handle over-time actions and updates (regen, auto-attacks, DoTs…);
- [x] Auto-attacking (infinite);
- [x] In-game time.

## What's coming next

See the _Unrealeased_ section of [CHANGELOG.md](CHANGELOG.md)

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

## How to run

Build then start a login server, then a game server. Now log in with a game client supporting protocol 656 and voilà!

## Credits

[l2auth][1] (by [@Ruk33][2]) and [L2jMobius][3].

[1]: https://github.com/Ruk33/l2auth
[2]: https://github.com/Ruk33
[3]: https://gitlab.com/MobiusDevelopment/L2J_Mobius/
[4]: https://legacy-lineage2.com/news/chronicle4_01.html
