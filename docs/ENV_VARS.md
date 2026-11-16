# Environment & Configuration Contract

This project reads no environment variables. The firmware is configured with constants at the top of `firmware/string_lights/string_lights.ino` (see `docs/firmware.md`, "Settings"), and the case with the PARAMETERS block in `hardware/case/uno_case_v2.py` (see `docs/case.md`).

## Required Variables

None.

## Optional Variables

None. The printer address used in `docs/OPERATIONS.md` (`192.168.1.85:7125`) is typed into commands by hand, not read from the environment.
