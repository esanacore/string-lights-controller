# Build log: 2026-11-15

Chronological record of the whole build, from idea to finished electronics. Photo names refer to [images/](images/) (all captioned in [gallery.md](gallery.md)).

## 1. Idea and component choice (12:05 to 12:15)

- Goal: get rid of the batteries in a West & Arrow string light and control it more ways than the built-in switch.
- Measured 3.3 V on the battery pack, which meant the batteries were drained (fresh 3 cells is about 4.5 V).
- Key early points: the Uno can't power the string from a GPIO pin; cheap strings often have no current-limit resistor, so don't feed raw 5 V; check for a controller chip and a polarity-flipping two-wire string.
- Considered the SainSmart 4-channel USB relay. It's a USB-controlled FT245 board (driven from a PC/Pi, not the Uno), clicks, and can't dim, so it was dropped.
- Settled on Uno + transistor low-side switch. First sketch (v1.0) written: PN2222 switch, optional 4N35 optocoupler to press a mode button, IR, serial.
- Photos: `01-parts-inventory-*`.

## 2. Remotes (12:15 to 12:17)

- Apple TV remote turned out to be a Siri Remote 2nd gen (A2540): Bluetooth, not usable with the Uno. Ruled out.
- Photos: `02-siri-remote-*`.

## 3. Opening the battery box (12:20 to 12:30)

- Box is 3xAA (not AAA) into a slide switch into the string. No chip, no mode button. So: no optocoupler needed, and PWM dimming works directly.
- Wiring: top-left contact (pack +) to the switch's left lug, string to the middle lug; pack − at the bottom-right spring.
- Current measurement: first read 0 because the red lead was in the 10A jack while the dial was on mA. Fixed the lead, measured **16 mA** with the meter in series across the open switch (low batteries).
- Photos: `03-battery-box-*`, `04-current-measurement`.

## 4. Breadboard prototype (12:30 to 13:05)

- Lights taken down to work on the bench.
- Transistor turned out to be marked **2N2222**, not PN2222; some 2N2222s have reversed outer legs. Diode test read 0.680 on both junctions (can't tell C from E), so we went with E-B-C and planned to confirm in circuit.
- Wire list (colors chosen for tracking): black GND, yellow D9, blue/green string −, red 5V and string +. Diode 1N4007 with the stripe toward the string.
- Photos: `05-*`, `06-breadboard-wiring-*`.

## 5. Software setup (13:04 to 13:20)

- Arduino IDE wasn't installed. Installed 2.3.11 with `winget install ArduinoSA.IDE.stable`.
- Computer-use control of the IDE didn't work at first (the app list hadn't refreshed), so this round was done by hand.
- Sketch saved to `C:\Users\esana\Claude\string_lights\`.
- Bug: floating D2 made the IR library print endless `ir: unmapped code 0x0`. Fixed in v1.1 with `ENABLE_IR = false` and dropping unknown-protocol frames.
- `on` printed `status: on` but no light. Root cause: the box's **slide switch was off**. After switching it on, the lights worked with the transistor rotated from the first orientation; Vce measured **50 mV**, confirming that orientation is correct.
- `dim`, `off` all worked.
- Photos: `07-*`.

## 6. Effects and first case (13:25 to 13:40)

- v1.2: 8 effects (solid, breathe, twinkle, candle, pulse, blink, wave, sunrise), speed control, gamma correction.
- First case generated in Python (manifold3d): base and lid for Uno + proto shield, PETG on the 0.6 mm nozzle.
- Decided to solder to the proto shield instead of using its stick-on mini breadboard; case lowered to 35.6 mm.
- The IDE autosaved its old copy over the new sketch (`unknown command: effect breathe`). Fixed by saving effects as a new sketch, `string_lights_fx`. Effects confirmed working.
- Photos: `08-proto-shield-*`, `09-ide-stale-sketch`.

## 7. Moving to the proto shield (13:39 to 15:25)

- Decided to cut the string leads about an inch above the battery box (so it can be re-spliced later) and wire the string straight to the shield. Polarity found by trial: reversed LEDs just don't light.
- Swapped the Elegoo Uno for the **RexQualis Uno** from the kit, since the shield came from the same kit. Same R3 layout, so the case still fits.
- Soldering, one step at a time, in the shield's plain-hole area (holes there are not connected to each other):
  1. Transistor: marked leg = collector (left, flat face toward you).
  2. 1k resistor from the base. **Mistake:** the pin 9 wire was first soldered to the transistor end of the resistor (bypassing it), and the base leg wasn't actually joined to the resistor. Meter read OL. Fixed by moving the pin 9 wire to the resistor's far lead and bending the base leg over to the resistor lead.
  3. Emitter to GND with a short wire. Beep test confirmed GND.
  4. Diode: plain end on the 5V strip, striped end in a plain hole.
  5. String + to the diode's striped lead, string − to the collector.
- Short checks: no beep 5V to GND, string ± to GND, base to GND. Pin 9 to base read about 1.0 kΩ.
- First power-up on the soldered shield: lights on.
- Photos: `10-*` through `17-*`.

## 8. v2 firmware: memory, buttons, matrix, IR (15:25 to 15:31)

- Screen options considered: 0.96 in I2C OLED (recommended, not in kit), LCD1602 (6 wires), **MAX7219 8x8 matrix (chosen: free, 3 data wires)**, or none.
- v2.0: EEPROM state with a 3 s save debounce, IR, two buttons, MAX7219 icons and bar graphs.
- Installed LedControl. Memory test passed: candle mode survived an unplug.

## 9. IR receiver investigation (15:31 to 16:30)

See [ir-receiver.md](ir-receiver.md) for the full trace. Summary: the module's header order didn't match its pad labels (beep-mapped: red = −, green = S, white = +); the wires were swapped at first; the remote is a 21-key NEC remote with no OK key (v2.1 remapped it); the module output sat low; a library-free test sketch, a dark test and end-to-end wiring tests showed the receiver reacts to bright light but never decodes the remote. Conclusion: faulty receiver, replacement needed. Along the way a wire in D1 (TX) caused `avrdude: not in sync` upload failures.

- Photos: `18-*`.

## 10. Power-cycle mode and the S1 button (16:15 to 16:41)

- v2.2: each power-up = lights on, full brightness, next effect (saved immediately so quick replugs still advance).
- S1 on the shield: beep test showed the two outer legs are factory-wired to GND; a white wire from an inner leg to the D4 pad. Short and long presses confirmed.
- Photos: `19-s1-to-d4-wire`.

## 11. Matrix on jumpers (16:42 to 16:51)

- Matrix input header: VCC GND DIN CS CLK. Jumpers to 5V, GND, D12, D10, D11. Breathe ring showed on first try.
- Sunrise looked broken: it took 10 minutes to brighten from dark. v2.3 shortened it to 30 s.
- Photos: `20-*`.

## 12. Case v2, flashing, power (16:51 to 17:06)

- Took over the IDE with computer use and flashed v2.3.
- Base raised to 49.6 mm. Lid got a matrix frame and S1 plunger tube.
- S1 measured with calipers. The first read (44.25 mm) was ambiguous; a zoom on the photo showed S1 in line with the power header, and a second measurement gave **3.75 mm** from that edge. Tube moved, OD cut to 8.8 mm.
- Power: the kit's 9 V barrel adapter is fine (7 to 12 V center positive; about 150 mA total load).
- Photos: `21-s1-caliper`.

## 13. Hardwiring the matrix (17:06 to 17:45)

- Pins snipped about 3 mm past the plastic strip (output header snipped too), tinned, wires soldered: red VCC, black GND, yellow DIN, white CS, brown CLK, 5 to 6 in long.
- Shield side: red/black fed down through holes onto the 5V/GND strips; data wires fed through plain holes and soldered onto the D10/D11/D12 header joints. No bridges between 9/10, 10/11, 11/12.
- Photos: `22-*`, `23-*`, `24-*`.

## 14. Matrix orientation (17:45 to 17:51)

- Heart showed lying on its side (the module is wired rotated 90°). v2.4 added `MATRIX_ROTATE` (set to 1) and `MATRIX_MIRROR`. Flashed with computer use.
- Verified: heart upright, twinkle's big sparkle top left (not mirrored).
- Photos: `25-*`, `26-*`.

## 15. Repo package and first case print (17:52 to 18:25)

- Full repo assembled and zipped: firmware history, CAD, 70 captioned photos, docs, ADRs, host syntax check.
- Case sliced with PrusaSlicer 2.7.2 CLI for the AD5M Pro (ZMOD Klipper): base, lid and plunger on one plate, PETG, 0.6 mm nozzle, 0.3 mm layers, ~1 h 40 m, ~65 g. Start gcode `START_PRINT EXTRUDER_TEMP=240 BED_TEMP=80`, end `END_PRINT`.
- Verified via Moonraker before printing: `klippy_state: ready`, axis limits −125..125 (centered origin). Uploaded and started with curl from PowerShell (`print_started: true`).

# 2026-11-16

## 16. Case v1 test fit

Assembled and running on the 9 V barrel adapter. Findings (photos `27-*`):

| Item | Result |
|---|---|
| Matrix window | Perfect fit, but nothing retains the module: hot glued |
| Uno | Holes line up with the standoffs; dropped in and hot glued (no screws yet) |
| Lid | Fits with an even seam; 0.25 mm/side lip has no retention in PETG: hot glued |
| Plunger | Works; guide hole needed a quick drill (elephant's foot) |
| USB cutout | About 6 mm off. Root cause: v1 used y = 44.4 (the USB-B connector's edge on the Uno drawing) as the center; the real center is ~38.4. Not blocking since power comes from the barrel jack |
| Barrel jack | Fits |
| Wire slot | Works, but the rest of the slot stays open |
| Corner roughness | Hot glue squeeze-out, not a print defect |

## 17. Case v2 design

Hardware inventory photographed (`28-hw-*`). v2 (`hardware/case/uno_case_v2.py`) uses Kadrick heat-set inserts throughout, adds removable plugs for the USB, barrel jack and wire slot, fixes the USB center, widens the plunger bore, and adds matrix mounting posts. Design reviewed independently before printing; see [case.md](case.md).

## Open items

- [ ] Replacement 1838 IR receiver; verify the remote codes with `ir_test` then the main sketch.
- [x] Case v1 printed, assembled, in use (glued).
- [ ] Measure the matrix module (3 numbers, see case.md) and print case v2.
- [ ] Choose a license.
- [ ] Optional: Home Assistant serial bridge, or move to an ESP32 + ESPHome for Wi-Fi.
- [ ] Optional: second button on D5.
