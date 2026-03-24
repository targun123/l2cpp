# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Send target's status when targetting;
- HP/MP/CP regeneration tick system;
- Inflict damage during auto-attacks;
- Better state machine handling.

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
