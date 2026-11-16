# ADR 0004: Step effects on each power-up

**Status:** accepted, 2026-11-15

## Context
The IR receiver turned out to be faulty, and the box is often powered from a switched outlet or charger.

## Decision
With `CYCLE_ON_POWERUP = true`, every power-up turns the lights on at full brightness on the next effect, saved to EEPROM at boot.

## Consequences
- A wall switch or replug becomes a mode button.
- Resuming exactly where it left off needs `CYCLE_ON_POWERUP = false` (planned once the IR receiver is replaced).
- Saving at every boot costs one EEPROM write per power-up; fine for 100k-cycle EEPROM.
