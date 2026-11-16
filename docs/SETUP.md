# Workstation Setup

## IDE Setup

This project follows Eric's Engineering Constitution. After cloning, run `git submodule update --init --recursive` so `constitution/` is present. AI assistants pick up `AGENTS.md` / `CLAUDE.md` automatically.

## Supported Platforms

- Windows 11 (PowerShell): firmware build/upload (Arduino IDE), printing commands. Used for the build.
- Linux (Ubuntu / Mint): tests, case generation, slicing, printing commands. CI runs on Ubuntu.
- macOS: should work for everything (same tools), not tested.

## Prerequisites

Firmware: Arduino IDE 2.x with **IRremote** (Armin Joachimsmeyer) and **LedControl** (Eberhard Fahle). Case and tests: Python 3.11+, `g++`, and `requirements-dev.txt`. Slicing: PrusaSlicer 2.7+.

<details>
<summary>Windows (PowerShell)</summary>

```powershell
winget install ArduinoSA.IDE.stable
git clone --recurse-submodules https://github.com/esanacore/string-lights-controller
```

</details>

<details>
<summary>Linux (bash)</summary>

```bash
git clone --recurse-submodules https://github.com/esanacore/string-lights-controller
cd string-lights-controller
python3 -m pip install -r requirements-dev.txt
sudo apt-get install -y g++ prusa-slicer   # Debian, Ubuntu, Mint, WSL
# Fedora: sudo dnf install -y gcc-c++ python3-pip prusa-slicer
# macOS:  xcode-select --install
```

</details>

## First Run

1. Open `firmware/string_lights/string_lights.ino`, select **Arduino Uno** and the board's port, upload.
2. Serial Monitor at 9600 baud, line ending **New Line**: you should see `string lights v2 ready`.
3. `bash tests/run_all.sh` should end with `All tests passed.`

Gotcha: the Arduino IDE autosaves its open buffer over files changed on disk. Open a changed sketch fresh (or under a new folder name) before uploading.
