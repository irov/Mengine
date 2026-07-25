#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CMAKE=cmake

if test -x "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -x "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION="${1:-}"

if test -z "${CONFIGURATION}"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

"${SCRIPT_DIR}/make_solution_xcode_macos.sh" "$@"

SOLUTION_DIR="${SCRIPT_DIR}/../../solutions/solution_xcode_macos/${CONFIGURATION}"

"${CMAKE}" --build "${SOLUTION_DIR}" --config "${CONFIGURATION}"
