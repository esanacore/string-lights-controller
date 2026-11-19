# Agent Handoff

This document helps transition work between different AI agent sessions or different agents.

**Before starting your session**, check `docs/SESSION_PLAN.md` for the previous agent's planned work and resumption notes, and `docs/MEMORY.md` to load durable codebase learnings and user preferences. The session plan captures *intent before work*; this handoff captures *state after work*. Only the latest handoff lives here: the lasting record of each session — what was asked, decided, and learned — is its summary in `docs/sessions/`, which you search before planning and add to before completing.

## How to Handoff

When you are finishing a task or session, record the state here or in a dedicated `HANDOFF.md` file:

1. **Current Status**: What was achieved?
2. **Next Steps**: The Next Steps procedure from the end of your last push (see the constitution's `AI_WORKFLOW.md`, "Next Steps Procedure"): a numbered checklist in the order things should happen, every step tagged `HUMAN`, `AGENT`, or `AUTOMATED`, and every step only a person can do — hardware, credentials, UI-only settings, approvals — called out up front and described well enough to follow cold.
3. **Known Blockers**: What issues were encountered?
4. **Context Hints**: Are there specific files or discussions the next agent should read?

Replace the entry below at the end of every push; only the latest handoff is
kept. `constitution/scripts/check_next_steps.sh` checks the procedure's
format.

## Handoff Template

### Session: 2026-11-16
- **Accomplishments**: Case v3 designed (no inserts, press-fit lid, pilot holes) after the v2 corner inserts went in off-center; v3 base sliced. Earlier: case v2 designed, reviewed and tested; v2b base print (with `MESH=MESH_DATA`) started; Engineering Constitution v1.56.0 adopted as a submodule with all required docs filled; tests consolidated under `tests/`; before/after write-up in `docs/CONSTITUTION_ADOPTION.md`.
- **Verification Run**: `bash tests/run_all.sh` (18 checks, all pass); `bash constitution/scripts/run_declared_tests.sh . --strict`; all `constitution/scripts/check_*.sh` scripts listed in `docs/CONSTITUTION_ADOPTION.md`.
- **Known Blockers**: lid geometry needs three matrix measurements; IR receiver module is dead.
- **Context Hints**: `hardware/case/uno_case_v2.py` (`MX_*` constants are estimates), `docs/case.md`, `docs/sessions/2026-11-16-case-v2-and-constitution-adoption.md`.

#### Next Steps

**Human action required:** steps 1, 2, 3 and 5.

1. [ ] **HUMAN** — Check the v3 gray print (started 2026-11-16 at 250 C as `print_v3/v3_case_full_set_PETG_0.6n_250C.gcode`), send photos, and test-fit the Uno with M2x6 and M3 self-tapping screws
   - **Why a human:** physical print and assembly on Eric's AD5M Pro.
   - **Done when:** the board screws down without stripping and the v2 plugs and plunger fit the new base.
2. [ ] **HUMAN** — Move the electronics out of the glued v1 case and measure the MAX7219 module
   - **Why a human:** needs calipers on the real part.
   - **Done when:** three numbers are sent: matrix face to PCB top (mm), mounting hole centers from the LED block edge (mm), and hole to hole spacing (mm).
3. [ ] **HUMAN** — Buy a replacement IR receiver (VS1838B or TSOP38238)
   - **Why a human:** a purchase.
   - **Done when:** the part is wired to D2 and `firmware/ir_test` prints codes.
4. [ ] **AGENT** — Set `MX_*` in `hardware/case/uno_case_v3.py`, rerun `bash tests/run_all.sh`, slice the v3 lid with `MESH=MESH_DATA`, add it to `print_v3/`, update the CHANGELOG.
5. [ ] **HUMAN** — Print the lid and press it on
   - **Why a human:** physical print and assembly.
   - **Done when:** the lid holds by friction, pops off at the notch, the matrix sits flush in the window, and S1 works through the plunger.
6. [ ] **AUTOMATED** — Constitution workflows run on every push; watch for a red run.
