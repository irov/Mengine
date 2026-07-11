#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_DIR="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${REPOSITORY_DIR}/solutions/downloads"

cmake -G "Unix Makefiles" \
    -S "${REPOSITORY_DIR}/cmake/Downloads" \
    -B "${BUILD_DIR}"

cmake --build "${BUILD_DIR}" \
    --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-8}"
