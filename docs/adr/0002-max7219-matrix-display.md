# ADR 0002: MAX7219 8x8 matrix as the display

**Status:** accepted, 2026-11-15

## Options
0.96 in I2C OLED (best, not on hand), LCD1602 from the kit (6 data wires plus a contrast pot, large), MAX7219 8x8 matrix from the kit, no screen.

## Decision
MAX7219 matrix: free, 3 data pins (D10 to D12), readable icons per effect.

## Consequences
- Text isn't practical; icons and bar graphs only.
- The module's wiring is rotated 90°, handled in firmware (`MATRIX_ROTATE`).
- Intensity kept at 1 since it shares USB power with the string.
