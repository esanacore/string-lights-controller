# IR receiver investigation

Module: RexQualis kit IR receiver board, silkscreen **RQ-S003**, metal-can 1838-type receiver, power LED (D1) and signal LED (D2), protection diode D3. Remote: 21-key NEC remote (Power, Mode, Mute, Play, Prev, Next, EQ, −, +, 0, Loop, USB SCAN, 1 to 9).

## Trace

| # | Test | Result | Meaning |
|---|---|---|---|
| 1 | First wiring by color | Wrong; signal on 5V, + on D2 | Possible damage starts here |
| 2 | Beep from the receiver's labeled pads to the 3-pin header | − = red, S = green; + didn't beep (through D3) | Header order differs from pad labels; green/white were swapped |
| 3 | Rewired: − GND, + 5V, S D2 | Signal LED solid red | Output held low |
| 4 | Remote into the phone camera | Purple flash | Remote works |
| 5 | Meter + to − | 4.8 V | Module powered |
| 6 | Library-free `ir_test` sketch | `pin stuck LOW` | Confirms the line is low |
| 7 | Pull green out of D2 | `idle` | The module is what pulls it low |
| 8 | Cover the receiver completely | `idle` | In the dark it rests high: it was swamped by window light |
| 9 | Shade it, remote into the lens | No `activity` | Can't decode the remote |
| 10 | Signal LED blinks every 2 s with `idle` prints | Green wire was in D1 (TX) | Moved to D2 |
| 11 | Jumper D2 to GND | `pin stuck LOW` | Socket 2 reaches the Uno |
| 12 | Bridge S to − at the module | `pin stuck LOW` | Green wire is continuous |
| 13 | Room light, 6 to 12 in, held buttons | Nothing | Receiver can't demodulate |

**Conclusion:** wiring, socket, remote and power all verified. The receiver reacts to bright light but never sees the 38 kHz remote. Most likely damaged by the first miswire (or faulty from the factory). Replace it.

## Replacement

- Any 38 kHz 1838 receiver (TL1838 / VS1838B), bare or on a module. Prefer a module that advertises reverse-polarity protection.
- Wire **− to GND, + to 5V, S to D2**. Beep-map the pins before power.
- Verify with `firmware/ir_test`: `idle` at rest, `activity: N changes` on key presses. Then load the main sketch and check that each key does its job (codes in [firmware.md](firmware.md)); unknown keys print their code.
- Keep the receiver out of direct sunlight; give it a window in the case.
