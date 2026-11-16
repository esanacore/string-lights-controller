# Firmware

Current: `firmware/string_lights/string_lights.ino` (v2.4). Libraries: **IRremote** v4.x (Armin Joachimsmeyer), **LedControl** (Eberhard Fahle). About 17.8 KB flash (55%), 788 B RAM.

## Controls

**Power-up** (`CYCLE_ON_POWERUP = true`): lights on, full brightness, next effect, saved to EEPROM immediately.

**S1 (D4):** short press = on/off, long press (≥700 ms) = next effect.
**Button 2 (D5, optional):** short = next brightness step (255/128/48/16), long = next speed.

**IR (21-key kit remote):**

| Key | Action | NEC code |
|---|---|---|
| Power | on/off | 0x45 |
| Mode / Next | next effect | 0x46 / 0x43 |
| Prev | previous effect | 0x40 |
| + / − | brighter / dimmer (hold repeats) | 0x09 / 0x15 |
| EQ | faster | 0x07 |
| USB SCAN | slower | 0x0D |
| Play/Pause | freeze/resume effect | 0x44 |
| Mute | matrix display on/off | 0x47 |
| 1 to 8 | jump to effect | 0x0C 0x18 0x5E 0x08 0x1C 0x5A 0x42 0x52 |
| 9 | full brightness | 0x4A |
| 0 | night mode (16/255) | 0x16 |

The codes are the standard ones for this remote and have **not** been verified on hardware yet (receiver pending). Unknown keys print `ir: unmapped code 0x..`.

**Serial (9600, newline):** `on`, `off`, `toggle`, `status`, `list`, `next`, `dim <0-255>`, `speed <1-10>`, `effect <name>`.

## Effects and icons

| # | Effect | Behavior | Matrix icon |
|---|---|---|---|
| 1 | solid | steady | filled square |
| 2 | breathe | smooth cosine fade, never fully dark | ring |
| 3 | twinkle | mostly bright with random quick dips | big + small sparkle |
| 4 | candle | smoothed random flicker | flame |
| 5 | pulse | double-beat heartbeat | heart |
| 6 | blink | on/off 50% | checker blocks |
| 7 | wave | linear fade fully off and back | curve |
| 8 | sunrise | 30 s ramp from dark, then holds | half sun |

Brightness passes through a square-law gamma so fades look even.

## Settings (top of sketch)

| Constant | Default | Meaning |
|---|---|---|
| `ENABLE_IR` | true | Start the IR receiver on D2 (D2 has a pull-up so an empty pin stays quiet) |
| `ENABLE_MATRIX` | true | Drive the MAX7219 |
| `CYCLE_ON_POWERUP` | true | Step effects on each power-up; set false to resume exactly where it left off |
| `MATRIX_INTENSITY` | 1 | 0 to 15 |
| `MATRIX_ROTATE` | 1 | Quarter turns clockwise. 1 = upright with the matrix wires pointing down |
| `MATRIX_MIRROR` | false | Flip if icons come out mirrored (verified not needed) |
| `SAVE_DELAY_MS` | 3000 | EEPROM write debounce |

## Version history

| Version | File | Change |
|---|---|---|
| v1.0 | (not kept) | First sketch: PN2222 power switch, optocoupler mode-button, IR, serial |
| v1.1 | `archive/v1.1_basic_ir-fix.ino` | `ENABLE_IR` switch + drop unknown-protocol IR noise (floating D2 flooded 0x0 codes) |
| v1.2 | `archive/v1.2_effects.ino` | 8 effects, speed, gamma, optocoupler code removed (box has no mode button) |
| v2.0 | (superseded same session) | EEPROM memory, 2 buttons, MAX7219 icons, IR for the 17-key remote |
| v2.1 | `archive/v2.1_remote-map.ino` | Remapped for the 21-key remote, pause, matrix on/off |
| v2.2 | `archive/v2.2_cycle-on-powerup.ino` | Power-cycle effect stepping at full brightness |
| v2.3 | `archive/v2.3_short-sunrise.ino` | Sunrise ramp 10 min to 30 s |
| v2.4 | `string_lights/string_lights.ino` | `MATRIX_ROTATE`/`MATRIX_MIRROR`; icons upright |

## Checking without a board

`tests/test_firmware_syntax.sh` (run by `bash tests/run_all.sh`) compiles every sketch on the host with g++ against stub headers in `tests/stubs/`, and `tests/test_firmware_static.py` checks the pin map, effect list, matrix rotation and memory settings. It catches syntax and declaration errors only; it is not a substitute for the Arduino compiler or hardware testing.
