# Test Plan

How this repository is tested, what it targets, and where the gaps are.

## Test Strategy

This is an embedded hardware project, so the pyramid is split between host-side automated checks and recorded bench verification.

- **Unit tests**: static checks on the firmware source (pin map, effect list, settings) and geometry checks on the case model. Location / command: `tests/test_firmware_static.py`, `tests/test_case_geometry.py`.
- **Integration tests**: every sketch (current, IR test, archived versions) is compiled on the host with g++ against stub Arduino headers (`tests/stubs/`). Location / command: `tests/test_firmware_syntax.sh`.
- **End-to-end tests**: manual bench and fit checks on the real hardware, recorded with dates and photos in `docs/build-log.md` and the traceability matrix. Not automatable without hardware-in-the-loop.

## How to Run Tests

- Full suite: `bash tests/run_all.sh`
- With coverage: not measured; see Coverage Targets.
- A single test or subset: `python3 tests/test_case_geometry.py`, `python3 tests/test_firmware_static.py`, or `bash tests/test_firmware_syntax.sh`

Setup: `pip install -r requirements-dev.txt` and a C++ compiler (`g++`).

## Coverage Targets

Line coverage isn't meaningful for a single Arduino sketch checked statically. The target is requirement coverage: every `MUST` requirement has an automated check or a dated manual verification.

| Scope | Metric | Floor |
| --- | --- | --- |
| MUST requirements | Automated or recorded manual verification | 100% |
| Case parts | Geometry checks (watertight, bed fit) | 100% of exported parts |
| Firmware sketches | Host syntax check | 100% of sketches that compile standalone |

## Continuous Coverage Evaluation

| Date | Overall coverage | Notes |
| --- | --- | --- |
| 2026-11-16 | 13 of 13 requirements with checks; 18 automated tests | Baseline at constitution adoption |

## Coverage Gap Log

| Gap ID | Area / behavior | Risk | Related requirement | Status | TODO ref |
| --- | --- | --- | --- | --- | --- |
| GAP-001 | No real Arduino (avr-gcc) compile in CI; host check uses stubs | med | NFR-004 | Open | TODO.md Testing |
| GAP-002 | Effect timing (`effectLevel`) not unit-tested | low | FR-002 | Open | TODO.md Testing |
| GAP-003 | Matrix post dimensions are estimates; lid fit unverified | med | FR-008 | Open | TODO.md Features |
| GAP-004 | IR remote codes unverified on hardware | low | FR-007 | Open | TODO.md Testing |
| GAP-005 | Home Assistant control not started (backlog, no requirement yet) | low | backlog | Open | TODO.md Features |

## Requirement Coverage

The authoritative mapping is `docs/REQUIREMENTS_TRACEABILITY.md`.
