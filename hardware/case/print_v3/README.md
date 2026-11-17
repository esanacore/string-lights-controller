# Case v3 print (Adventurer 5M Pro, ZMOD Klipper)

`v3_case_base_PETG_0.6n.gcode`: v3 base only. Dried gray PETG, 0.6 mm nozzle, 0.3 mm layers, 240/80 C.
PrusaSlicer 2.7.2 estimate ~1 h 16 m, ~16.5 m filament. Start gcode loads the saved mesh (`MESH=MESH_DATA`).

Profile changes from v2: `avoid_crossing_perimeters = 1` and `wipe = 1` to cut stringing across the rim.

The plunger and the three plugs from the v2 print are reused (v3 keeps the same cutouts and S1 position).
The lid is not sliced yet: it waits on the HW-109 matrix measurements (see ../../../docs/case.md).

```bash
curl -4 -F "file=@v3_case_base_PETG_0.6n.gcode" -F "print=true" http://<printer-ip>:7125/server/files/upload
```

Hardware: M2x6 socket heads at the two holes next to the barrel jack, M3 self-tapping pan heads
(6 or 8 mm) at the two far holes. Drive them gently; they cut their own threads in the pilot holes.
