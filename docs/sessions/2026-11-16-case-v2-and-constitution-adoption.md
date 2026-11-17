# Session: Case v2 and constitution adoption

- **Date:** 2026-11-16
- **Agent:** Claude (Opus 5.5) via claude.ai, case review by a Fable 5.2 subagent
- **Branch / PR:** main
- **Topics:** case v2, heat-set inserts, plugs, bed mesh, MESH_DATA, START_PRINT, engineering constitution, submodule, tests, traceability
- **Related:** 2026-11-15-arduino-string-lights-build, FR-008, FR-010, docs/CONSTITUTION_ADOPTION.md

## Request

Redesign the case using Eric's Kadrick heat-set inserts with removable plugs for the USB, barrel jack and wire slot; optimize it and get a stronger model to review it. Print the base now and measure the matrix later. Then install the engineering constitution as a submodule, bring the repo into compliance, and document the before and after.

## Context

Case v1 worked but was hot glued: no screw retention, a 6 mm USB offset, and a plunger that needed drilling. Eric sent photos of his fastener inventory so the design uses screws he owns.

## Decisions

- Board mounts on M2/M3 inserts in standoffs; lid on M3 inserts in corner bosses with countersunk screws (agent).
- Fixes from the independent review: M3 head clearance at hole 2, standoff moved off solder tails, slot plug changed to an H-section (agent).
- Print the base first; the lid waits for three matrix measurements (Eric).
- Start gcode passes `MESH=MESH_DATA` after the first v2 print started from the wrong height (Eric caught it and canceled).
- Constitution v1.56.0 pinned as `constitution/`; tests moved from `tools/` to `tests/` with a single `tests/run_all.sh`; ADRs renumbered so ADR-0001 is the constitution's own record decision.

## Discoveries

- ZMOD `START_PRINT` does not load a mesh unless `MESH=` is given. The previous night's print only worked because a mesh was still active from calibration.
- `docs/troubleshooting.md` and `docs/TROUBLESHOOTING.md` collide on case-insensitive filesystems; merged into one.

## Outcome

Case v2 design with geometry tests, the v2b base print running, and the constitution adoption with filled docs, 18 passing tests and the before/after write-up in `docs/CONSTITUTION_ADOPTION.md`.

## Open Questions

- Matrix measurements (face to PCB, hole offset from block, hole spacing) needed for the lid.
- IR receiver replacement.

## Addendum (2026-11-16, evening)

The v2b base printed with good walls and cutouts, and all three plugs fit (the barrel plug after trimming bridge sag). The corner-boss insert holes filled with oozed wet PETG; Eric heat-pressed M3 inserts into them anyway, several went in off-center, and he decided to drop heat-set inserts. Case v3 (`uno_case_v3.py`) removes the corner bosses, makes the lid press-fit with crush ribs, and uses pilot holes for M2x6 socket heads and M3 self-tappers (Claude's pick from Eric's fastener stock). The v3 base is sliced for dried gray PETG with `avoid_crossing_perimeters` and `wipe` on.
