# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Get world actors when entering world;
- Broadcast world actor deletion when leaving world;
- Make corpses disappear after a few seconds;
- Inflict damage with a magic skill;
- HP/MP/CP regeneration tick system.

## [0.11.0] - 2026-04-03

### Added

- Damage can be inflicted and actor status is broadcasted to listeners
- The actor dies when HP reaches zero

### Fixed

- TargetClear was not working anymore

## [0.10.0] - 2026-04-03

### Added

- Monster targets display correct HP bar
- Groundwork for status broadcasting to listeners

### Changed

- Reworked HP/MP/CP/Weight from ActorStatus to Stats+ComputedStats

### Fixed

- Cannot attack oneself

## [0.9.0] - 2026-03-31

### Added

- Can now send system messages to player, with argument support
- Player is greeted upon entering the world

## [0.8.0] - 2026-03-31

### Added

- Characters created are restored as long as the server doesn't stop

### Changed

- Moved code related to character previews into World
- Some packets have been moved to their own implementation file

## [0.7.0] - 2026-03-28

### Added

- Dual weapons (swords, fists and bare fists) now inflict two hits as per their animation

### Changed

- AttackAction keeps track of the initial target so that the action completes even if player changes target
- AttackAction is more generic around the attacker type (Character vs. NPC/Monster)

### Removed

- NPC doesn't scream anymore when hit

## [0.6.0] - 2026-03-26

### Added

- Simple packet broadcasting to all characters
- Remove player and its characters from World on connection closed

### Changed

- Automatic test character now gets a unique name on creation
- Packet logs indicate to/from which session they are related to

## [0.5.0] - 2026-03-25

### Added

- First ECS systems implementation 
- Characters' attack stance toggles off after 5s

## [0.4.0] - 2026-03-24

### Added

- Action system is more fleshed out: current/next action algorithm, queueing actions, interruptible…

## [0.3.1] - 2026-03-24

### Fixed

- Game loop doesn't overuse the CPU when there's nothing much going on

## [0.3.0.1] - 2026-03-24

### Changed

- Replace `OptionalRef = std::option<std::reference_wrapper<T>>` with my own `OptRef` implementation (to avoid writing the ugly `opt->get()` all the damn time)

## [0.3.0] - 2026-03-23

### Added

- In-game time
- Can trigger sunset/sunrise events

## [0.2.0] - 2026-03-23

### Added

- NPCs can talk
- auto-attacking (attack stance, timed to atk speed)
- Arrows are automatically equipped with bow (PoC)
- Can look items up from ItemTemplate::id in ItemStorage

### Fixed

- Dual weapons display correctly (dual fists, dual swords)
