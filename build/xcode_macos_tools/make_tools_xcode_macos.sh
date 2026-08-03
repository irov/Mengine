#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPOSITORY_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"

if test "$#" -lt 1; then
    echo "Usage: $0 <Configuration>" >&2
    exit 2
fi

CONFIGURATION="$1"
SOLUTION_DIR="${REPOSITORY_DIR}/solutions/tools_xcode_macos/${CONFIGURATION}"

if test "$(uname -m)" != "arm64"; then
    echo "Mengine macOS tools support Apple Silicon arm64 only"
    exit 1
fi

cmake \
    -G Xcode \
    -S "${REPOSITORY_DIR}/cmake/Tools_Xcode_MacOS" \
    -B "${SOLUTION_DIR}" \
    -DCMAKE_BUILD_TYPE:STRING="${CONFIGURATION}" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="${CONFIGURATION}" \
    -DCMAKE_OSX_ARCHITECTURES:STRING=arm64 \
    -DMENGINE_DEPENDENCIES_PROJECT:STRING=Depends_Xcode_MacOS
