#!/usr/bin/env bash
# Host-side syntax check of every firmware sketch against stub Arduino headers.
# Catches syntax/declaration errors without a board or the Arduino toolchain.
# It does NOT replace an Arduino compile (see docs/TEST_PLAN.md, GAP-001).
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
root="$here/.."
fail=0
check() {
  local sketch="$1" tmp
  tmp="$(mktemp -d)"
  cp "$sketch" "$tmp/sketch.cpp"
  if g++ -std=c++17 -fsyntax-only -I"$here/stubs" -include "$here/stubs/IRremote.hpp" "$tmp/sketch.cpp"; then
    echo "PASS  $sketch"
  else
    echo "FAIL  $sketch"; fail=1
  fi
  rm -rf "$tmp"
}
check "$root/firmware/string_lights/string_lights.ino"
check "$root/firmware/ir_test/ir_test.ino"
# Archived versions that compile standalone (v1.1 relied on the Arduino IDE's
# automatic prototypes and is intentionally excluded).
for f in "$root"/firmware/archive/v1.2*.ino "$root"/firmware/archive/v2.*.ino; do check "$f"; done
exit $fail
