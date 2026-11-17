# Requirements Traceability Matrix

Links each requirement in `docs/PRODUCT_REQUIREMENTS.md` to its tests and verification status. Hardware behavior that cannot be automated is verified manually on the bench; the date and evidence are recorded. Gaps are in `docs/TEST_PLAN.md`.

## Conventions

- **Level**: `MUST`, `SHOULD`, `COULD`, or `WON'T`.
- **Status**: `Not Started`, `In Progress`, `Verified`, or `Deferred`.

## Functional Requirements

| Requirement ID | Level | Description | Acceptance Criteria | Verifying Tests | Status |
| --- | --- | --- | --- | --- | --- |
| FR-001 | MUST | String powered and switched/dimmed by the Uno | FR-001-AC-1, FR-001-AC-2 | `tests/test_firmware_static.py::test_pin_map`; manual bench test 2026-11-15 (`on`/`dim`/`off`, photos 07-*, 24-*) | Verified |
| FR-002 | MUST | Eight effects, brightness, speed | FR-002-AC-1, FR-002-AC-2 | `test_firmware_static.py::test_effects_list`, `::test_sunrise_duration`; manual 2026-11-15 | Verified |
| FR-003 | MUST | State survives power loss | FR-003-AC-1 | `test_firmware_static.py::test_power_cycle_and_memory`; manual 2026-11-15 (candle survived unplug) | Verified |
| FR-004 | MUST | S1 short/long press | FR-004-AC-1 | `test_firmware_static.py::test_pin_map`; manual 2026-11-15 (bench), 2026-11-16 (v1 case plunger) | Verified |
| FR-005 | MUST | Upright matrix icons | FR-005-AC-1 | `test_firmware_static.py::test_matrix_orientation`; manual 2026-11-15 (photos 26-*) | Verified |
| FR-006 | SHOULD | Power-up steps the effect | FR-006-AC-1 | `test_firmware_static.py::test_power_cycle_and_memory`; manual 2026-11-15 | Verified |
| FR-007 | SHOULD | 21-key IR remote | FR-007-AC-1 | `test_firmware_static.py::test_pin_map` (D2); codes unverified: receiver faulty (`docs/ir-receiver.md`) | Deferred |
| FR-008 | MUST | Enclosure (v3) | FR-008-AC-1, -AC-2, -AC-3 | `tests/test_case_v3_geometry.py` (`test_v3_cutouts_match_v2_plugs`, `test_v3_lid_does_not_collide_except_ribs`, `test_v3_lid_press_fit`, `test_v3_lid_clears_slot_plug`); v2 base printed 2026-11-16, plugs fit; v3 base and lid pending print | In Progress |
| FR-009 | SHOULD | Serial commands | FR-009-AC-1 | Syntax only (`tests/test_firmware_syntax.sh`); manual 2026-11-15 | Verified |

## Non-Functional Requirements

| Requirement ID | Level | Description | Acceptance Criteria | Verifying Tests | Status |
| --- | --- | --- | --- | --- | --- |
| NFR-001 | MUST | No secrets, no network | NFR-001-AC-1 | `constitution/scripts/check_secrets.sh` (CI) | Verified |
| NFR-002 | MUST | Printable on the AD5M Pro | NFR-002-AC-1 | `test_case_geometry.py::test_parts_watertight`, `::test_fits_printer_bed`; v1 printed 2026-11-15 | Verified |
| NFR-003 | MUST | Within component limits | NFR-003-AC-1, -AC-2 | Manual measurement 2026-11-15 (Vce 50 mV, 16 mA string current on low batteries); `test_case_geometry.py::test_screw_lengths_fit_insert_holes` | Verified |
| NFR-004 | MUST | One-command automated checks | NFR-004-AC-1 | `tests/run_all.sh` (CI: constitution-tests) | Verified |

## Coverage Summary

| Metric | Count |
| --- | --- |
| Total requirements | 14 |
| Verified | 11 |
| In progress | 1 |
| Deferred | 1 |
| Not started | 1 |
