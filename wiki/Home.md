# Home

String Lights Controller turns a West & Arrow battery LED string into an Arduino-controlled light with eight effects, a push button, an 8x8 mode display, memory across power loss, and a 3D printed case. It's for anyone who wants to hack a cheap battery light string into something smarter with parts from a starter kit.

## What this project does

An Arduino Uno R3 drives the LED string through a 2N2222 low-side switch on PWM pin 9. A button (S1) changes effects and brightness, flipping the power off and on steps to the next effect, and a MAX7219 matrix shows an icon for the current mode. Settings survive power loss in EEPROM.

## Getting started

See `docs/SETUP.md` for parts, wiring and flashing, and `docs/wiring.md` for the circuit.

## How it works

See `docs/ARCHITECTURE.md` for the block diagram and `docs/firmware.md` for the effect engine. The case is a parametric Python model in `hardware/case/`, described in `docs/case.md`.

## Where things live

- `firmware/`: the current sketch, the IR test sketch, and every flashed version in `archive/`
- `hardware/case/`: case models, STLs and sliced gcode per version
- `docs/`: build log, wiring, BOM, ADRs, requirements, test plan, photos
- `tests/`: the automated suite (`bash tests/run_all.sh`)
- `constitution/`: Eric's Engineering Constitution (submodule)

## See also

- `docs/build-log.md`: the full build story with photos
- `docs/TROUBLESHOOTING.md`: problems hit and how they were fixed
- `docs/CONSTITUTION_ADOPTION.md`: what changed when this repo adopted the constitution
