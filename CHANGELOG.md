# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Send target's status when targetting;
- HP/MP/CP regeneration tick system;
- Inflict damage during auto-attacks;
- Better state machine handling;
- `World::update` CPU usage reduction.

## [0.2.0] - 2026-03-23

### Added

- NPCs can talk
- auto-attacking (attack stance, timed to atk speed)
- Arrows are automatically equipped with bow (PoC)
- Can look items up from ItemTemplate::id in ItemStorage

### Fixed

- Dual weapons display correctly (dual fists, dual swords)
