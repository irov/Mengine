#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

exec /bin/bash "${SCRIPT_DIR}/make_solution.bash" Debug
