# Troubleshooting

Every problem hit during the build, its cause and fix. See also `docs/ir-receiver.md` for the full IR investigation.

Every problem hit during the build, its cause and fix.

| Symptom | Cause | Fix |
|---|---|---|
| Battery pack read 3.3 V | Drained cells (fresh 3xAA is about 4.5 V) | Nothing; it's why the project started |
| Meter read 0 mA | Red lead in the 10A jack with the dial on mA (or a blown mA fuse) | Lead in the jack that matches the dial |
| Meter "keeps climbing" on a breadboard row | Probe not touching metal inside the hole | Probe the bare component legs |
| Serial Monitor flooded with `ir: unmapped code 0x0` | Nothing on D2; the floating pin decoded as noise | `ENABLE_IR` switch, ignore UNKNOWN protocol, pull-up on D2 |
| Serial says `status: on`, lights dark | Battery box slide switch was off | Switch on (or bypass the box entirely) |
| `unknown command: effect breathe` | IDE autosaved its old buffer over the updated file | Save new versions under a new sketch name/folder |
| Serial Monitor "Not connected" | Opened during the upload | Close and reopen it, or reselect the port |
| Pin 9 to base read OL | Base leg not joined to the resistor; holes on the shield's plain area don't connect | Bend the leads together and solder them |
| Resistor bypassed | Pin 9 wire on the transistor end of the resistor | Wire goes to the resistor's far lead |
| `avrdude: stk500_getsync() not in sync` | Wire in D1 (TX) | Keep D0/D1 empty; unplug/replug, reselect COM port |
| IR module LED solid red, `pin stuck LOW` | See [ir-receiver.md](ir-receiver.md): faulty receiver (also swamped by window light) | Replace the 1838 receiver; keep it out of direct sun |
| Sunrise "doesn't work" | 10 minute ramp from dark | v2.3: 30 s |
| Matrix icons rotated 90° | Module's row/column wiring | v2.4: `MATRIX_ROTATE = 1` |
| Computer use couldn't find Arduino IDE | Claude desktop app's app list was cached before the install | Restart the desktop app, or do it by hand |
| Computer use access expired | 30 minutes without activity | Re-approve the IDE |

## Lessons

- Map module pins with a meter instead of trusting printed labels; this IR module's header order differed from its pad labels.
- Meter and continuity checks before every power-up caught the bypassed resistor.
- Version sketches by folder name; the IDE will silently save stale buffers.
- Ask for one physical measurement instead of guessing positions from photos (S1 was 5 mm off the first estimate).
