# String Lights Controller

<!-- CONSTITUTION_START -->
[![Eric's Engineering Constitution](https://img.shields.io/badge/Eric's%20Engineering%20Constitution-Adopted-blue)](https://github.com/esanacore/engineering-constitution)
<!-- CONSTITUTION_END -->

A West & Arrow battery string light (originally 3xAA, single slide switch) converted to a hardwired, Arduino-controlled light with effects, a button, an 8x8 LED icon display, power-loss memory, and IR remote support.

![Finished electronics: twinkle effect with the icon on the matrix](docs/images/26-twinkle-verified.jpg)

## What it does

- Runs from USB or a 9 V barrel adapter. No more batteries.
- 8 effects: solid, breathe, twinkle, candle, pulse (heartbeat), blink, wave, sunrise (30 s fade-in).
- Dimming and speed control (PWM through a 2N2222 low-side switch).
- **Power-cycle mode:** every power-up turns the lights on at full brightness on the next effect.
- **S1 button** on the proto shield: short press = on/off, long press = next effect.
- **8x8 MAX7219 matrix** shows an icon for the current effect, a bar graph when brightness/speed changes, and a single dot in standby.
- **EEPROM memory:** on/off, effect, brightness and speed survive power cuts.
- **IR remote** (21-key NEC kit remote) is mapped in firmware. The kit receiver turned out to be faulty, so this is waiting on a replacement 1838 receiver.
- **Serial control** at 9600 baud for a future Home Assistant bridge.
- 3D-printed PETG case: base, lid with matrix window, and a plunger for S1.

## Repo layout

| Path | What |
|---|---|
| [firmware/string_lights/](firmware/string_lights/string_lights.ino) | Current firmware (v2.4). This is what's flashed. |
| [firmware/ir_test/](firmware/ir_test/ir_test.ino) | Library-free IR receiver test sketch |
| [firmware/archive/](firmware/archive/) | Every earlier firmware version from the build |
| [hardware/case/](hardware/case/) | Case generators (`uno_case.py` v1, `uno_case_v2.py` v2), STLs and sliced gcode per version |
| [docs/wiring.md](docs/wiring.md) | Final schematic, pin map, wire colors |
| [docs/bom.md](docs/bom.md) | Parts list |
| [docs/firmware.md](docs/firmware.md) | Controls, settings, serial commands, version history |
| [docs/case.md](docs/case.md) | Case design, print settings, assembly |
| [docs/build-log.md](docs/build-log.md) | Full chronological log of the 2026-11-15 build |
| [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) | Every problem hit and how it was resolved |
| [docs/ir-receiver.md](docs/ir-receiver.md) | IR receiver investigation and replacement plan |
| [docs/adr/](docs/adr/) | Design decisions |
| [docs/gallery.md](docs/gallery.md) | All 104 build and case photos with captions |
| [tests/](tests/run_all.sh) | Automated suite: firmware syntax and static checks, case geometry checks (`bash tests/run_all.sh`) |
| [constitution/](https://github.com/esanacore/engineering-constitution) | Eric's Engineering Constitution, pinned as a git submodule |
| [docs/PRODUCT_REQUIREMENTS.md](docs/PRODUCT_REQUIREMENTS.md) | Requirements (FR/NFR) with [traceability](docs/REQUIREMENTS_TRACEABILITY.md) and a [test plan](docs/TEST_PLAN.md) |
| [docs/CONSTITUTION_ADOPTION.md](docs/CONSTITUTION_ADOPTION.md) | Before and after: what adopting the constitution changed in this repo |
| [demo.html](demo.html) | Interactive in-browser simulator of the firmware (effects, S1, matrix icons) |

## Quick start

1. Install Arduino IDE 2.x (`winget install ArduinoSA.IDE.stable`).
2. Library Manager: install **IRremote** (Armin Joachimsmeyer) and **LedControl** (Eberhard Fahle).
3. Open `firmware/string_lights/string_lights.ino`, select **Arduino Uno** and the board's COM port, upload.
4. Wire it per [docs/wiring.md](docs/wiring.md).

## Status

| Area | State |
|---|---|
| Electronics | Done, soldered on the proto shield, tested |
| Firmware v2.4 | Flashed and verified (effects, button, matrix orientation, memory) |
| IR | Code ready; needs a replacement receiver |
| Case | v1 printed and in use (glued). v2 (heat-set inserts, plugs, USB fix) designed and reviewed; base printing (v2b gcode, loads the saved bed mesh), lid waits on 3 matrix measurements |
| Home Assistant | Not started (serial bridge planned) |

## Running the tests

Supported platforms: Linux, macOS, and Windows through WSL or Git Bash (the scripts are bash). The firmware itself is flashed from the Arduino IDE on any OS.

Linux, macOS, or Windows (WSL / Git Bash):

```bash
git clone --recurse-submodules https://github.com/esanacore/string-lights-controller
cd string-lights-controller
pip install -r requirements-dev.txt   # needs g++ and Python 3 too
bash tests/run_all.sh
```

## Engineering Constitution

This repo follows [Eric's Engineering Constitution](https://github.com/esanacore/engineering-constitution), pinned as the `constitution/` submodule. Agents start from [AGENTS.md](AGENTS.md); the session log lives in [docs/sessions/](docs/sessions/README.md). [docs/CONSTITUTION_ADOPTION.md](docs/CONSTITUTION_ADOPTION.md) shows the repo before and after adoption.

## License

Apache License 2.0. See [LICENSE](LICENSE) and [NOTICE](NOTICE).
