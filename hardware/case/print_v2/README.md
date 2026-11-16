# Case v2 print (Adventurer 5M Pro, ZMOD Klipper)

`v2b_case_base_plugs_PETG_0.6n.gcode`: base + plunger + USB plug + barrel plug + slot plug on one plate.
PETG, 0.6 mm nozzle, 0.3 mm layers, 4 walls, 20% gyroid, 240/80 C. PrusaSlicer 2.7.2 estimate ~1 h 46 m, ~18.8 m filament.

The **lid is not sliced yet**: its matrix posts need three measurements of the HW-109 module first (see ../../../docs/case.md).

## v2 vs v2b

The first v2 slice started with `START_PRINT EXTRUDER_TEMP=240 BED_TEMP=80`. On ZMOD that loads no bed mesh, so the first layer went down too high and the print was canceled. v2b is identical except the start line passes `MESH=MESH_DATA` (the saved mesh profile). Only v2b is kept here; delete any old `v2_` file on the printer.

Upload and start from Linux:

```bash
curl -4 -F "file=@v2b_case_base_plugs_PETG_0.6n.gcode" -F "print=true" http://<printer-ip>:7125/server/files/upload
```
