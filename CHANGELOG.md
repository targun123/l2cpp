# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Simple broadcasting to all online characters in a short distance from emitter;
- Send target's status when targetting;
- HP/MP/CP regeneration tick system;
- Inflict damage during auto-attacks.

## [0.7.0] - 2026-03-28

### Added

- Dual weapons (swords, fists and bare fists) now inflict two hits as per their animation

### Changed

- AttackAction keep track of the initial target so that the action completes even if player changes target
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
