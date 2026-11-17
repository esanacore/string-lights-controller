#!/usr/bin/env bash
# Full suite: firmware syntax, firmware static checks, case geometry.
set -euo pipefail
cd "$(dirname "$0")/.."
bash tests/test_firmware_syntax.sh
python3 tests/test_firmware_static.py
python3 tests/test_case_geometry.py
python3 tests/test_case_v3_geometry.py
echo "All tests passed."
