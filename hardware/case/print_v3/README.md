# Case v3 print (Adventurer 5M Pro, ZMOD Klipper)

`v3_case_full_set_PETG_0.6n.gcode`: base, lid, plunger and all three plugs on one plate, so everything matches. Dried gray PETG, 0.6 mm nozzle, 0.3 mm layers, 240/80 C.
PrusaSlicer 2.7.2 estimate ~2 h 07 m, ~23.8 m filament. Start gcode loads the saved mesh (`MESH=MESH_DATA`).

Profile changes from v2: `avoid_crossing_perimeters = 1` and `wipe = 1` to cut stringing across the rim.

The lid is printed with the estimated matrix post positions (`MX_*` in `uno_case_v3.py`). If the matrix posts miss the HW-109 holes, measure the module and reprint only the lid. Plugs have 0.5 mm top clearance (v2: 0.3) for bridge sag.

```bash
curl -4 -F "file=@v3_case_full_set_PETG_0.6n.gcode" -F "print=true" http://<printer-ip>:7125/server/files/upload
```

Hardware: M2x6 socket heads at the two holes next to the barrel jack, M3 self-tapping pan heads
(6 or 8 mm) at the two far holes. Drive them gently; they cut their own threads in the pilot holes.

`v3_case_full_set_PETG_0.6n_250C.gcode` is the file actually printed on 2026-11-16: identical except 250 C, for the OEM steel nozzle. Made with:

```bash
sed -e 's/EXTRUDER_TEMP=240/EXTRUDER_TEMP=250/' -e 's/^M109 S240/M109 S250/' -e 's/^M104 S240/M104 S250/' \
  v3_case_full_set_PETG_0.6n.gcode > v3_case_full_set_PETG_0.6n_250C.gcode
```
