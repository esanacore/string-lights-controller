# ADR 0002: 2N2222 low-side switch instead of a relay

**Status:** accepted, 2026-11-15

## Context
The string draws tens of mA at about 4.5 V. Options on hand: SainSmart 4-channel USB relay, IRF520 MOSFET (kit), 2N2222/PN2222 transistors.

## Decision
Switch the string's negative side with a 2N2222 driven from D9 through 1 kΩ. Feed the positive side from 5 V through a 1N4007 (about 4.3 V).

## Consequences
- PWM dimming and effects are possible; a relay can only do on/off and clicks.
- The IRF520 isn't logic-level and would only half turn on from 5 V.
- The diode keeps the resistor-less string near its original battery voltage.
- Vce(sat) measured 50 mV, so losses are negligible.
