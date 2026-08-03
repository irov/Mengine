#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

if test "$#" -lt 1; then
    echo "Usage: $0 <Configuration>" >&2
    exit 2
fi

CONFIGURATION="$1"

"${SCRIPT_DIR}/make_tools_xcode_macos.sh" "${CONFIGURATION}"
cmake --build "${REPOSITORY_DIR}/solutions/tools_xcode_macos/${CONFIGURATION}" --config "${CONFIGURATION}" --parallel
