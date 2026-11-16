# Session Log

One summary per working session: what was asked, the context the conversation
established, the decisions and their reasoning, what was learned, and what
shipped. Commits and the changelog record *what changed*; this log records
*why*, so a later agent or person can find it instead of rediscovering it.

**Before starting work**, read the newest rows below, then search the log for
the task's topic and read the summaries that match:

```bash
bash constitution/scripts/find_sessions.sh <keyword> [<keyword>...]
```

The scripts are bash and the commands are the same on every platform: Linux
and macOS run them as written; on Windows, run them from Git Bash or WSL.

**Before completing non-trivial work**, copy `TEMPLATE.md` to `YYYY-MM-DD-<slug>.md`, fill
it in, and rebuild the table below with
`bash constitution/scripts/build_session_index.sh .` (it is generated from the entries; do not
edit rows by hand). Summaries are append-only:
correct a mistake with a dated note, and record a change of direction in a
new summary that links the old one. A summary is not a transcript and never
carries secrets, credentials, personal data, or customer data.

`bash constitution/scripts/check_session_summaries.sh .` checks the format and
that every summary is listed here.

## Index

Newest first.

| Date | Session | Topics |
| --- | --- | --- |
| 2026-11-16 | [Case v2 and constitution adoption](2026-11-16-case-v2-and-constitution-adoption.md) | case v2, heat-set inserts, plugs, bed mesh, MESH_DATA, START_PRINT, engineering constitution, submodule, tests, traceability |
| 2026-11-15 | [Arduino string lights build](2026-11-15-arduino-string-lights-build.md) | string lights, arduino uno, 2N2222, PWM, effects, MAX7219, EEPROM, IR receiver, case v1, PETG, AD5M Pro |
