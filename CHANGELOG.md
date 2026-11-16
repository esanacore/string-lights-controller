# Changelog

All notable user-facing changes to this project are documented here. This project follows semantic versioning. Firmware and case carry their own version numbers (firmware v2.4, case v2); see `docs/firmware.md` and `docs/case.md`.

## Unreleased

### Added

- Case v2 lid: sliced once the matrix module is measured (see `docs/case.md`).
- `docs/CONSTITUTION_ADOPTION.md`: before and after metrics for the constitution adoption.

### Fixed

- Case v2 gcode now loads the saved bed mesh (`START_PRINT ... MESH=MESH_DATA`). The first v2 slice printed from the wrong height and was canceled; it is replaced by `v2b_case_base_plugs_PETG_0.6n.gcode`.

## 0.2.0 - 2026-11-16

### Added

- Eric's Engineering Constitution adopted as the `constitution/` submodule (v1.56.0) with governance docs, CI workflows, session log and requirements traceability.
- Automated test suite (`tests/run_all.sh`): firmware syntax checks, firmware pin/setting checks, case geometry checks (watertight, fits the bed, lid/base interference, screw-to-insert depths, hole alignment).
- Case v2 design (`hardware/case/uno_case_v2.py`): heat-set inserts for lid, board and matrix; press-fit plugs for the USB cutout, barrel cutout and wire slot; independently reviewed before printing. Base, plunger and plugs sliced and printing.
- Apache-2.0 license and NOTICE.
- Case v1 test-fit results and hardware inventory photos.

### Fixed

- USB cutout position: v1 used the USB-B connector's edge (y = 44.4) as its center; v2 uses 38.4.

### Changed

- ADRs renumbered to follow the constitution's ADR 0001 (record architecture decisions).
- `docs/troubleshooting.md` merged into `docs/TROUBLESHOOTING.md` (the two names collide on case-insensitive filesystems).
- Host syntax check moved from `tools/` to `tests/`.

## 0.1.0 - 2026-11-15

### Added

- Hardwired controller for West & Arrow string lights: Uno R3 + soldered proto shield, 2N2222 low-side PWM switch, S1 button, MAX7219 8x8 matrix.
- Firmware v2.4: 8 effects, dimming, speed, EEPROM memory, power-cycle effect stepping, matrix icons, IR remote mapping, serial commands.
- Case v1 (PETG, Adventurer 5M Pro) and full build log with 70 photos.
