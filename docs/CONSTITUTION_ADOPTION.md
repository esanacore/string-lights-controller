# Constitution Adoption: Before and After

This repo started life as a weekend build log: firmware, photos, a case model and a pile of notes, pushed to GitHub on 2026-11-15. On 2026-11-16 it adopted [Eric's Engineering Constitution](https://github.com/esanacore/engineering-constitution) v1.56.0 as a git submodule and was brought into compliance with everything it requires.

This page is the honest diff. The point is to show what the constitution adds to a small hobby project, not to pretend a string light needs enterprise process. Everything below was measured with the constitution's own checkers against the last pre-adoption commit (`cb6bf6b`) and against the adopted tree.

## The numbers

| Measure | Before (`cb6bf6b`) | After |
|---|---|---|
| Tracked files (excluding the submodule) | 131 | 184 |
| Markdown docs | 15 | 47 |
| CI workflows | 0 | 9 |
| Automated test files | 1 (`tools/check_sketch.sh`) | 7 under `tests/` |
| Automated checks run | 1 sketch syntax check, run by hand | 18 checks in one command, run in CI on every push |
| Written requirements | 0 | 14 (10 functional, 4 non-functional) |
| Requirements traced to a verifying test | 0 | 13 of 14 (FR-010 Home Assistant is honestly marked not started) |
| Documented test gaps | none written down | 5 (GAP-001 to GAP-005 in the test plan) |
| ADRs | 3 | 4 (constitution's ADR-0001 plus the three project decisions) |
| Session log | none | 2 summaries with a generated index |
| `check_compliance.sh` | 7 required, 14 recommended, 3 product files missing | 0 / 0 / 0 |
| `check_secrets.sh` .gitignore gaps | 6 | 0 |
| `run_declared_tests.sh --strict` | fails: no test plan declared | passes |
| `check_next_steps.sh` | no handoff to check | 6 steps, 4 tagged HUMAN with a "done when" |

## What existed before

The pre-adoption repo was already decent by hobby standards: a README, wiring and BOM docs, a firmware guide, a long build log, a troubleshooting page, three ADRs, 85 photos, every flashed firmware version archived, and a host-side syntax check script. Tree at `cb6bf6b`:

```
docs/            8 files + 3 ADRs + 85 images
firmware/        current sketch, IR test, 6 archived versions
hardware/case/   v1 generator, STLs, gcode
tools/           check_sketch.sh + 3 stub headers
README, LICENSE, NOTICE, CHANGELOG
```

What it did not have: any statement of what the thing is required to do, any way to tell whether a change broke something besides re-flashing and looking at it, any CI, and nothing telling a future agent (or future Eric) what was in flight and what only a human can do next.

## What the constitution added, and what each piece bought

**A requirements spine.** `docs/PRODUCT_REQUIREMENTS.md` writes down the ten things the light actually has to do (PWM dimming, eight effects, S1 short and long press, power-cycle stepping, EEPROM memory, the matrix icons, IR, the case fit, serial control, Home Assistant) plus four non-functional ones. `docs/REQUIREMENTS_TRACEABILITY.md` points each one at the test or dated hardware check that proves it. Writing this exposed that IR (FR-007) can't be verified at all until the receiver is replaced, and that Home Assistant (FR-010) is a wish, not a feature. Both are now visible instead of implied.

**Tests that encode lessons from the build.** The single syntax script became a suite that runs in one command (`bash tests/run_all.sh`) and in CI:

- Every firmware version in the archive is syntax checked, not just the current one.
- Static checks pin down things that bit us during the build: the pin map, the effect list, `MATRIX_ROTATE` (icons were sideways until v2.4), the power-cycle and EEPROM settings, and the 30 s sunrise (it was accidentally 10 minutes).
- Case geometry tests check that every part is watertight and fits the bed, that the USB cutout is centered on the real port (v1 was 6 mm off), that the lid doesn't collide with the base, that lid holes line up with the insert bosses, and that the screws Eric owns fit the insert holes.

None of these need a board or a printer, so a bad edit gets caught before anything is flashed or printed.

**A test plan with honest gaps.** `docs/TEST_PLAN.md` declares the full-suite command (which is what lets CI run it) and lists what is not automated: hardware-in-the-loop effect timing, real IR decoding, print fit, and so on. Before, those gaps existed but lived only in my head.

**CI gates.** Nine workflows now run on every push: tests, compliance, secrets, architecture, doc freshness, env vars, OTS inventory, constitution version, and wiki publishing. For a hobby repo the cost is zero and it means a broken sketch or a committed secret gets flagged automatically.

**Secret hygiene.** The project has no secrets, but the old `.gitignore` had six gaps (`.env`, keys, `credentials.json`, `.netrc`, Terraform state). Those are closed so a future Home Assistant token can't slip in.

**Continuity for the next session.** `docs/AGENT_HANDOFF.md` ends with a Next Steps checklist where every step is tagged HUMAN, AGENT or AUTOMATED. For a hardware project that matters a lot: most next steps are physical (check the print, measure the matrix, buy an IR receiver) and now each one says exactly what "done" looks like. `docs/MEMORY.md` keeps the gotchas we paid for (IDE autosave clobbering sketches, D0/D1 breaking uploads, the bed mesh not loading without `MESH=MESH_DATA`). The session log in `docs/sessions/` records why decisions were made, not just what changed.

**Inventory and operations docs.** `docs/OTS_SOFTWARE.md` lists every off-the-shelf piece (IRremote, LedControl, Arduino core, PrusaSlicer, Klipper/ZMOD, and the Python libraries with pinned versions in `requirements-dev.txt`). `docs/SETUP.md`, `docs/COMMAND_REFERENCE.md` and `docs/OPERATIONS.md` cover setup, every command used, and how to print and flash.

**A demo page.** `demo.html` is a single-file simulator of the firmware: all eight effects on a 20-bulb string, S1 click and hold, brightness and speed, the power-cycle step, and the matrix icons. Anyone can see what the light does without building it.

## Things adoption itself turned up

- The bootstrap added `docs/TROUBLESHOOTING.md` next to the existing `docs/troubleshooting.md`. Those two names collide on Windows and macOS, so they were merged into one file.
- The constitution reserves ADR-0001 for its own "record architecture decisions" ADR, so the project ADRs were renumbered to 0002 to 0004.
- The old syntax check was in `tools/`, which the constitution's test conventions don't look at. It moved to `tests/` and grew into the suite above.

## What it cost

Agent time while the case base printed. Most of the added files are short and specific to this project. The rule documents themselves live in the submodule, so the repo doesn't carry copies of them, and updating the constitution later is a submodule bump.

## What it didn't change

The firmware, the wiring and the case designs are exactly what they were. Adoption added structure around the project, not process inside it. A trivial doc fix still follows the constitution's lightweight path for trivial changes.

## Reproducing the numbers

```bash
git clone --recurse-submodules https://github.com/esanacore/string-lights-controller
cd string-lights-controller
for s in check_compliance check_secrets check_readme check_next_steps check_session_summaries; do
  bash constitution/scripts/$s.sh .
done
bash constitution/scripts/check_traceability.sh docs/PRODUCT_REQUIREMENTS.md docs/REQUIREMENTS_TRACEABILITY.md
bash constitution/scripts/run_declared_tests.sh . --strict

# Before: check out the pre-adoption commit in a separate folder and run the same scripts from the submodule
git worktree add /tmp/before cb6bf6b
bash constitution/scripts/check_compliance.sh /tmp/before
```
