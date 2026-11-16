# Wiring

## Schematic (text)

```
Uno 5V ──►|── 1N4007 ──┬── string + (wire that went to the box's switch)
          (stripe      │
           toward      │   LED string (no current-limit resistor of its own)
           string)     │
                       └── string − ──┐
                                      │ collector (marked leg)
Uno D9 ── 1kΩ ── base (middle leg)  2N2222
                                      │ emitter
Uno GND ──────────────────────────────┘

Shield S1:  outer legs = GND (factory), inner leg ── D4 (INPUT_PULLUP)

MAX7219 8x8 (HW-109):  VCC─5V  GND─GND  DIN─D12  CLK─D11  CS─D10

IR receiver (pending replacement):  S─D2  +─5V  −─GND
```

The diode drops about 0.7 V, so the string sees roughly 4.3 V, close to three fresh AA cells. The string has no resistor of its own and relies on the supply voltage, so do not feed it straight 5 V.

## Pin map

| Uno pin | Function |
|---|---|
| D2 | IR receiver signal (optional) |
| D4 | Button 1 = shield S1 (to GND when pressed) |
| D5 | Button 2 (optional, not fitted) |
| D9 | PWM to 1 kΩ to 2N2222 base |
| D10 | MAX7219 CS |
| D11 | MAX7219 CLK |
| D12 | MAX7219 DIN |
| 5V / GND | Shield center strips |

Do not connect anything to **D0/D1**. A wire on D1 (TX) blocked uploads during the build.

## Wire colors (as built)

| Run | Color |
|---|---|
| D9 to resistor far lead | red |
| Emitter to GND | red |
| S1 inner leg to D4 | white |
| Matrix VCC | red |
| Matrix GND | black |
| Matrix DIN (D12) | yellow |
| Matrix CS (D10) | white |
| Matrix CLK (D11) | brown |
| String +/− | original clear string leads (+ was tagged) |

Matrix leads are about 5 to 6 in (12 to 15 cm) so the lid can be set beside the box.

## Transistor pinout

The part is marked **2N2222** (TO-92). With the flat printed face toward you, legs down, this one is **C B E** left to right on the shield (marked leg = collector). A diode-mode meter test read 0.680 V on both junctions, so the orientation was confirmed by measuring Vce = **50 mV** when on (saturated).

## Measurements

| What | Value |
|---|---|
| Battery pack, low batteries | 3.33 V |
| String current on low batteries | 16 mA |
| Vce(sat), lights on | ~50 mV |
| Pin 9 pad to base | ~1.0 kΩ |
| IR module supply | 4.8 V |
| S1 center | 25.5 mm from USB-end edge, 3.75 mm from power-header edge |

## Power

- **USB**: fine.
- **Barrel jack**: 7 to 12 V center positive. The kit's 9 V adapter is ideal. Total draw is about 150 mA, so the onboard regulator barely warms. A 5 V barrel adapter will not work (regulator dropout).
