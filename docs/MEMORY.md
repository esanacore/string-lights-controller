# Project Memory

This file contains durable memories, codebase learnings, user preferences, and key architectural decisions. AI agents read this file at the start of each session to align with past context, and update it (at the user's discretion) at the end of a session.

> [!IMPORTANT]
> **User Discretion**: Do not add or edit entries in this file without presenting them to the user for review. The user has absolute discretion over what memories are retained.

## User Preferences & Styling Choices

- Eric wants hands-on work given one step at a time, then waits for his result before the next step.
- Docs and messages: concise, casual, plain prose, no em dashes.
- Public projects use Apache-2.0. Projects meant for profit stay private.
- Small focused changes; never redesign working code or a printed part unless asked.
- Print first, measure later is fine: the base can print before the lid measurements exist.

## Codebase Learnings & Gotchas

- The Arduino IDE autosaves its open buffer over the sketch file. Save each new firmware version to a new folder or it can clobber an update.
- A floating D2 floods IRremote with `0x0` UNKNOWN codes. `ENABLE_IR` gates the receiver and UNKNOWN protocols are filtered.
- Anything wired to D0/D1 breaks uploads (`avrdude: stk500_getsync`). Keep those pins free.
- The MAX7219 module is mounted rotated 90°: `MATRIX_ROTATE 1` in the sketch fixes the icons.
- The current IR receiver module is dead (likely damaged by an early miswire). A replacement is needed before FR-007 can be verified.
- USB-B port center is 38.4 mm from the board edge on this Uno, not 44.4 (that is the connector edge). v1 case cutout was 6 mm off.
- Heat-set insert holes for a 0.6 mm nozzle: M3 4.2 mm, M2 2.9 mm.
- ZMOD `START_PRINT` loads no mesh unless `MESH=<profile>` is passed. The saved profile is `MESH_DATA`.
- PrusaSlicer 2.7 CLI has no `--arrange` with this setup; build the plate in trimesh and slice with `--dont-arrange`.

## Active Project Decisions

- Low-side NPN switch (2N2222) on D9 with a 1N4007 dropping 5 V to about 4.3 V for the resistor-less LED string (ADR-0002).
- MAX7219 8x8 matrix as the mode display (ADR-0003).
- Power-cycle stepping through effects plus EEPROM memory (ADR-0004).
- Case v2 uses heat-set inserts and removable press-fit plugs; lid waits on three matrix measurements.
- The repo adopts Eric's Engineering Constitution as a `constitution/` submodule.
- Small isolated islands near the top of a PETG print (like 7 mm corner bosses) collect ooze when the filament is wet; insert holes in them can fill completely. Dry PETG and avoid tiny islands at the rim.
- Bridged tops of wall cutouts sag a few tenths of a mm; plugs may need the drooped strands trimmed.
- Printer is back on the OEM 0.6 mm steel nozzle (the Phaetus high-flow clogged); print PETG at 250 C. Re-run the bed mesh at 80 C and save it as `MESH_DATA` after any nozzle change.
- Printer upload commands must `cd` to the gcode first and chain with `&&`; an upload from the wrong folder sends an empty file that still reports `print_started: true`.
