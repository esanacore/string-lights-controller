"""Static checks on the current firmware: pin map and settings match the docs and hardware.

These catch accidental edits to values the wiring depends on. Run: python3 tests/test_firmware_static.py
"""
import re, sys, pathlib

SRC = (pathlib.Path(__file__).resolve().parent.parent / "firmware/string_lights/string_lights.ino").read_text()

def const(name):
    m = re.search(rf"\b{name}\s*=\s*([^;]+);", SRC)
    assert m, f"{name} not found"
    return m.group(1).strip()

def test_pin_map():                       # FR-001, FR-004, FR-005, FR-007
    assert const("PIN_POWER") == "9"      # PWM pin wired to the 1k base resistor
    assert const("PIN_BTN1") == "4"       # shield S1
    assert const("PIN_IR") == "2"
    assert (const("PIN_MX_CS"), const("PIN_MX_CLK"), const("PIN_MX_DIN")) == ("10", "11", "12")

def test_effects_list():                  # FR-002
    names = re.search(r"EFFECT_NAMES\[EFFECT_COUNT\]\s*=\s*\{([^}]*)\}", SRC).group(1)
    effects = re.findall(r'"(\w+)"', names)
    assert effects == ["solid", "breathe", "twinkle", "candle", "pulse", "blink", "wave", "sunrise"]
    assert SRC.count("PROGMEM") >= 1 and len(re.findall(r"//\s*\w+:", SRC.split("ICONS")[1].split("};")[0])) == 8

def test_matrix_orientation():            # FR-005 (verified upright on hardware 2026-11-15)
    assert const("MATRIX_ROTATE") == "1"
    assert const("MATRIX_MIRROR") == "false"

def test_power_cycle_and_memory():        # FR-003, FR-006
    assert const("CYCLE_ON_POWERUP") == "true"
    assert "EEPROM.put" in SRC and "EEPROM_MAGIC" in SRC
    assert int(const("SAVE_DELAY_MS")) >= 1000      # debounce EEPROM writes

def test_sunrise_duration():              # FR-002 (30 s ramp, not 10 min)
    assert "periodMs(30000UL)" in SRC

if __name__ == "__main__":
    fails = 0
    for name, fn in list(globals().items()):
        if name.startswith("test_"):
            try:
                fn(); print(f"PASS  {name}")
            except AssertionError as e:
                fails += 1; print(f"FAIL  {name}: {e}")
    sys.exit(1 if fails else 0)
