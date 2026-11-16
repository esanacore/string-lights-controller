# OTS Software Inventory

Off-the-shelf software this project depends on. Structure follows the intent of IEC 62304 SOUP / FDA OTS guidance, generalized. Update in the same change that adds, removes or upgrades a dependency.

## Conventions

- **Component ID** `OTS-nnn`, never reused. **Name** exactly as declared in the manifest. **License** as SPDX. **Risk** Low / Medium / High. **Status** Active / Evaluating / Removed.

## Managed Dependencies

Declared in `requirements-dev.txt` (case generator and tests only; nothing here ships to the device).

| Component ID | Name | Version | Supplier / Maintainer | Purpose | License | Risk | Verification | Anomaly Review | Update Policy | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| OTS-001 | manifold3d | 3.5.5 | elalish/manifold project | CSG modeling for the case generator | Apache-2.0 | Low | `tests/test_case_geometry.py` exercises booleans, volume and export; v1 printed and fit | github.com/elalish/manifold/issues, reviewed 2026-11-16 | pinned exact | Active |
| OTS-002 | trimesh | 5.2.1 | mikedh/trimesh project | STL export and watertight checks | MIT | Low | `tests/test_case_geometry.py::test_parts_watertight` | github.com/mikedh/trimesh/issues, reviewed 2026-11-16 | pinned exact | Active |
| OTS-003 | numpy | 2.5.3 | NumPy project | Mesh arrays for trimesh | BSD-3-Clause | Low | Indirect through the geometry tests | github.com/numpy/numpy/security, reviewed 2026-11-16 | pinned exact | Active |

## System-Level OTS

| Component ID | Name | Version | Supplier / Maintainer | Purpose | License | Risk | Verification | Anomaly Review | Update Policy | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| OTS-101 | Arduino AVR core (incl. EEPROM library) | bundled with Arduino IDE 2.3.11 | Arduino | Compiler, runtime, EEPROM on the Uno | LGPL-2.1-or-later | Medium | Firmware built and verified on hardware 2026-11-15 | github.com/arduino/ArduinoCore-avr/issues, reviewed 2026-11-16 | IDE board manager; updates deferred during the build | Active |
| OTS-102 | IRremote | 4.x (Library Manager) | Armin Joachimsmeyer | IR decoding (NEC) | MIT | Low | Compiles and runs; decoding unverified (receiver faulty, GAP-004) | github.com/Arduino-IRremote/Arduino-IRremote/issues, reviewed 2026-11-16 | Library Manager, manual | Active |
| OTS-103 | LedControl | 1.0.6 (Library Manager) | Eberhard Fahle | MAX7219 driver | MIT | Low | Icons verified on hardware 2026-11-15 | github.com/wayoda/LedControl/issues, reviewed 2026-11-16 | Library Manager, manual | Active |
| OTS-104 | Arduino IDE | 2.3.11 | Arduino | Build and upload | AGPL-3.0-only | Low | Used for every flash in the build log | github.com/arduino/arduino-ide/issues, reviewed 2026-11-16 | winget, manual | Active |
| OTS-105 | PrusaSlicer | 2.7.2 | Prusa Research | Slicing case gcode | AGPL-3.0-only | Low | v1 printed successfully | github.com/prusa3d/PrusaSlicer/issues, reviewed 2026-11-16 | Distro package | Active |
| OTS-106 | Klipper / Moonraker (ZMOD) | ZMOD 1.7.5 | Klipper, Arksine, ghzserg | Printer firmware and upload API | GPL-3.0-only | Low | Uploads/starts confirmed (`print_started: true`) | github.com/ghzserg/zmod/issues, reviewed 2026-11-16 | Printer-side, manual | Active |
| OTS-107 | g++ | system | GNU | Host syntax check of sketches | GPL-3.0-or-later | Low | `tests/test_firmware_syntax.sh` | n/a | CI image | Active |
