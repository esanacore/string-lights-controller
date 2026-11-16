# Session: Arduino string lights build

- **Date:** 2026-11-15
- **Agent:** Claude (Opus 5.5) via claude.ai, with computer use on Eric's laptop
- **Branch / PR:** main, commits eb491f4, f89edbe, cb6bf6b
- **Topics:** string lights, arduino uno, 2N2222, PWM, effects, MAX7219, EEPROM, IR receiver, case v1, PETG, AD5M Pro
- **Related:** ADR-0002, ADR-0003, ADR-0004, FR-001 to FR-009

## Request

Eric wanted his West & Arrow battery string lights (3xAA box with a slide switch) hardwired to an Arduino with several ways to control them, built step by step, then fully documented with every photo and pushed to a public repo.

## Context

Hardware on hand: RexQualis Uno R3, a solderable proto shield, 2N2222 transistors, 1N4007 diodes, a MAX7219 8x8 module, a 21-key IR remote kit, push buttons. The LED string has no current limiting resistor, so it expects about 4.5 V from 3xAA.

## Decisions

- Low-side 2N2222 switch on D9 with a 1k base resistor and a 1N4007 in series to drop 5 V to about 4.3 V (agent proposed, Eric approved).
- Effects engine with eight modes, S1 short/long press, EEPROM memory with a save delay, and power-cycle stepping so a wall switch can change modes.
- MAX7219 matrix shows the current mode icon; rotated 90° in firmware.
- Case v1 printed in PETG on the AD5M Pro with a 0.6 mm nozzle.
- Repo is public under Apache-2.0 (Eric's choice for things he shares).

## Discoveries

- Several wiring faults found by measurement: pin 9 wire on the wrong end of the resistor, a base joint not soldered, the slide switch left off, the meter lead in the 10 A jack, a wire on D1 blocking uploads.
- The IR receiver decodes nothing even in the dark; treated as dead hardware.
- The IDE autosave can overwrite updated sketches.

## Outcome

Firmware v2.4 flashed and working. Case v1 printed and assembled with hot glue. Repo pushed with docs, 85 photos, firmware archive, STLs and gcode.

## Open Questions

- Replacement IR receiver needed.
- Case v1 has no window retention and the USB cutout is off; addressed by case v2.
