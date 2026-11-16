# Command Reference

## Tests

| Command | What it does |
| --- | --- |
| `bash tests/run_all.sh` | Full suite |
| `bash tests/test_firmware_syntax.sh` | Host syntax check of every sketch |
| `python3 tests/test_firmware_static.py` | Pin map and settings checks |
| `python3 tests/test_case_geometry.py` | Case v2 geometry checks |

## Case

| Command | What it does |
| --- | --- |
| `cd hardware/case && python3 uno_case_v2.py` | Regenerate v2 STLs into `stl_v2/` |
| `prusa-slicer --export-gcode --load print_v2/ad5m_petg_06.ini --dont-arrange --output out.gcode part.stl` | Slice for the AD5M Pro |

## Firmware serial commands (9600 baud, newline)

`on`, `off`, `toggle`, `status`, `list`, `next`, `dim <0-255>`, `speed <1-10>`, `effect <solid|breathe|twinkle|candle|pulse|blink|wave|sunrise>`

## Printer (Moonraker on the AD5M Pro)

| Command | What it does |
| --- | --- |
| `curl -4 http://192.168.1.85:7125/server/info` | Health check (`klippy_state: ready`) |
| `curl -4 -F "file=@FILE.gcode" -F "print=true" http://192.168.1.85:7125/server/files/upload` | Upload and start |
| `curl -4 -X POST http://192.168.1.85:7125/printer/print/cancel` | Cancel |

## Constitution

| Command | What it does |
| --- | --- |
| `bash constitution/scripts/check_compliance.sh .` | Governance file check |
| `bash constitution/scripts/run_declared_tests.sh --strict .` | Run the declared test command |
| `bash constitution/scripts/find_sessions.sh <keyword>` | Search the session log |
