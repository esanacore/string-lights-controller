# TODO

Living roadmap. Open items are `[ ]`, in progress `[~]`, done `[x]`.

## Features

- [ ] Replace the faulty IR receiver (any 38 kHz 1838) and verify the 21-key remote codes with `firmware/ir_test`, then the main sketch (FR-007).
- [ ] Print the v3 base (gray PETG) and test-fit the Uno with M2x6 / M3 self-tappers (FR-008).
- [ ] Case v3 lid: measure the HW-109 (LED face to PCB, hole positions and diameter, overhang past the block), set `MX_*` in `uno_case_v3.py`, slice, print (FR-008).
- [ ] Set `CYCLE_ON_POWERUP = false` once IR works, so power-up resumes the last state (ADR 0004).
- [ ] Home Assistant control: serial bridge on a nearby host, or move to an ESP32 + ESPHome.
- [ ] Optional second button on D5 (brightness/speed); firmware already supports it.

## Technical Debt

- [ ] `uno_case.py` (v1) and `uno_case_v2.py` duplicate helpers; fold v1 into git history once v2 is verified.
- [ ] Matrix post dimensions are estimates until measured (GAP-003).

## Refactoring

- [ ] Split the firmware into effects / inputs / display files once it grows past one screen per concern.

## Testing

- [ ] GAP-001: add a real Arduino compile in CI (`arduino/compile-sketches` action with IRremote and LedControl).
- [ ] GAP-002: unit-test effect timing (`effectLevel`) on the host with a fake `millis()`.
- [ ] GAP-004: record verified IR codes once the receiver is replaced.

## Documentation

- [ ] Add photos of case v2 printed and assembled to `docs/gallery.md` and the build log.

## Nice-to-Have

- [ ] Gamma table instead of the square-law approximation.
- [ ] Printed diffuser for the matrix window.

## Done

- [x] Hardwired controller built and verified on hardware (2026-11-15).
- [x] Case v1 printed and in use (2026-11-16).
- [x] Repository published, Apache-2.0, constitution adopted (2026-11-16).
