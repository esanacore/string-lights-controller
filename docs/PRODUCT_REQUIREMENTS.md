# Product Requirements

Requirements for the string lights controller. IDs are referenced by `docs/REQUIREMENTS_TRACEABILITY.md`, tests and ADRs.

## Requirement Levels

`MUST` is required for the product to be acceptable. `SHOULD` is expected unless there is a documented reason. `COULD` is optional. `WON'T` is explicitly out of scope for now.

## Requirement Identifiers

- Functional requirements use the prefix `FR-` (for example, `FR-001`).
- Non-functional requirements use the prefix `NFR-` (for example, `NFR-001`).
- Acceptance criteria may carry sub-identifiers (for example, `FR-001-AC-1`).

## Product Summary

A West & Arrow LED string light, originally powered by a 3xAA box with a slide switch, rewired to an Arduino Uno R3 so it runs from USB or a 9 V adapter, offers lighting effects, and can be controlled several ways. Single user, single device, no network.

## Functional Requirements

### Power and switching

**FR-001** `MUST` run the string without batteries, switched and dimmed by the Uno.

- Acceptance criteria:
  - `FR-001-AC-1`: with USB or a 7 to 12 V barrel adapter, `on` lights the string and `off` turns it off.
  - `FR-001-AC-2`: `dim 40` visibly dims the string.

### Effects

**FR-002** `MUST` provide the effects solid, breathe, twinkle, candle, pulse, blink, wave and sunrise, with adjustable brightness and speed.

- Acceptance criteria:
  - `FR-002-AC-1`: each effect is selectable and visibly distinct.
  - `FR-002-AC-2`: sunrise reaches full brightness in about 30 s at speed 5.

**FR-003** `MUST` keep on/off, effect, brightness and speed across a power loss.

- Acceptance criteria:
  - `FR-003-AC-1`: after a change, waiting 5 s and cycling power restores the state (subject to FR-006).

### Controls

**FR-004** `MUST` support the shield's S1 button: short press toggles on/off, long press (about 0.7 s) selects the next effect.

- Acceptance criteria:
  - `FR-004-AC-1`: both press types work with the case closed (via the plunger).

**FR-005** `MUST` show the current effect as an upright icon on the 8x8 matrix, a bar graph when brightness or speed changes, and a single dot when off.

- Acceptance criteria:
  - `FR-005-AC-1`: the pulse heart is upright and the twinkle sparkle is top-left with the matrix wires pointing down.

**FR-006** `SHOULD` step to the next effect at full brightness on every power-up, so a wall switch works as a mode button.

- Acceptance criteria:
  - `FR-006-AC-1`: each unplug/replug advances one effect in list order.

**FR-007** `SHOULD` accept the 21-key kit IR remote (power, mode/next/prev, +/−, EQ/USB SCAN, 1 to 9, 0, play, mute).

- Acceptance criteria:
  - `FR-007-AC-1`: each mapped key performs its action; unmapped keys print their code.

**FR-009** `SHOULD` accept serial commands at 9600 baud: `on`, `off`, `toggle`, `status`, `list`, `next`, `dim <0-255>`, `speed <1-10>`, `effect <name>`.

- Acceptance criteria:
  - `FR-009-AC-1`: every command changes state as named and prints `status:`.

**FR-010** `COULD` be controlled from Home Assistant.

- Acceptance criteria:
  - `FR-010-AC-1`: HA can switch, dim and pick an effect.

### Enclosure

**FR-008** `MUST` house the Uno, shield and matrix in a printed case with the USB and barrel jack reachable, the string leads strain-relieved, and a lid that can be opened and closed repeatedly (v2).

- Acceptance criteria:
  - `FR-008-AC-1`: USB-B and barrel plugs seat fully through their cutouts.
  - `FR-008-AC-2`: lid fastens with screws into heat-set inserts and does not collide with the base.
  - `FR-008-AC-3`: unused cutouts can be closed with removable plugs.

## Non-Functional Requirements

### Security

**NFR-001** `MUST` hold no secrets and expose no network surface.

- Acceptance criteria:
  - `NFR-001-AC-1`: no credentials in the repository; the device has only USB serial and IR inputs.

### Reliability

**NFR-002** `MUST` print on the FlashForge Adventurer 5M Pro (0.6 mm nozzle, PETG) without supports, as closed solids that fit the 220 mm bed.

- Acceptance criteria:
  - `NFR-002-AC-1`: every exported part is watertight and under 215 mm on every axis.

**NFR-003** `MUST` stay within component limits: string at about 4.3 V (via the 1N4007), transistor saturated when on, total draw well under the USB 500 mA fuse; screws never bottom out in their insert holes.

- Acceptance criteria:
  - `NFR-003-AC-1`: measured Vce(sat) about 50 mV; total draw about 150 mA.
  - `NFR-003-AC-2`: screw length minus clamped thickness is shorter than each insert hole.

### Testability

**NFR-004** `MUST` have automated checks for firmware syntax, the pin map and case geometry, run by one command.

- Acceptance criteria:
  - `NFR-004-AC-1`: `bash tests/run_all.sh` passes.

## Explicit Non-Goals

- Per-LED addressing (the string is a single two-wire channel).
- Wi-Fi or cloud control on the Uno (would need an ESP32; see TODO).
- Battery operation.
