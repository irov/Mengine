#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_DIR="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"
CONFIGURATION="${1:-}"

if [[ -z "${CONFIGURATION}" ]]; then
    echo "Usage: $(basename -- "$0") <Debug|Release>" >&2
    exit 2
fi

BUILD_DIR="${REPOSITORY_DIR}/solutions/dependencies_unix/${CONFIGURATION}"

cmake -G "Unix Makefiles" \
    -S "${REPOSITORY_DIR}/cmake/Depends_Unix" \
    -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE:STRING="${CONFIGURATION}" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="${CONFIGURATION}"

cmake --build "${BUILD_DIR}" \
    --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-8}" \
    --config "${CONFIGURATION}"
