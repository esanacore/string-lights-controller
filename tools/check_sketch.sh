#!/usr/bin/env bash
# Host-side syntax check for the firmware. Compiles with g++ against stub headers,
# so it catches syntax/declaration errors without a board or the Arduino toolchain.
# It does NOT replace an Arduino compile or a hardware test.
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
sketch="${1:-$here/../firmware/string_lights/string_lights.ino}"
tmp="$(mktemp -d)"
cp "$sketch" "$tmp/sketch.cpp"
g++ -std=c++17 -fsyntax-only -I"$here/stubs" -include "$here/stubs/IRremote.hpp" "$tmp/sketch.cpp"
echo "OK: $sketch"
# Note: archive/v1.1 calls printStatus() before defining it and relies on the Arduino
# IDE's automatic prototypes, so it fails this host check but compiled and ran on the board.
