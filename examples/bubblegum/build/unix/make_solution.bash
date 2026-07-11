#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"
CONFIGURATION="${1:-}"

if [[ -z "${CONFIGURATION}" ]]; then
    echo "Usage: $(basename -- "$0") <Debug|Release>" >&2
    exit 2
fi

cmake -G "Unix Makefiles" \
    -S "${PROJECT_DIR}/cmake/Unix" \
    -B "${PROJECT_DIR}/solutions/solution_unix/${CONFIGURATION}" \
    -DCMAKE_BUILD_TYPE:STRING="${CONFIGURATION}" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="${CONFIGURATION}" \
    -DMENGINE_DEPENDENCIES_PROJECT:STRING=Depends_Unix
