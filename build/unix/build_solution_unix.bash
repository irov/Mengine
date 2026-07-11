#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_DIR="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"
CONFIGURATION="${1:-}"

if [[ -z "${CONFIGURATION}" ]]; then
    echo "Usage: $(basename -- "$0") <Debug|Release>" >&2
    exit 2
fi

BUILD_DIR="${REPOSITORY_DIR}/solutions/solution_unix/${CONFIGURATION}"

cmake -G "Unix Makefiles" \
    -S "${REPOSITORY_DIR}/cmake/Unix" \
    -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE:STRING="${CONFIGURATION}" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="${CONFIGURATION}" \
    -DMENGINE_DEPENDENCIES_PROJECT:STRING=Depends_Unix

cmake --build "${BUILD_DIR}" \
    --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-8}" \
    --config "${CONFIGURATION}"
