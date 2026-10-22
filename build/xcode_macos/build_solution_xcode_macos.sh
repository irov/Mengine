#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIGURATION="${1:-}"

if test -z "${CONFIGURATION}"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

"${SCRIPT_DIR}/make_solution_xcode_macos.sh" "$@"

SOLUTION_DIR="${SCRIPT_DIR}/../../solutions/solution_xcode_macos/${CONFIGURATION}"

xcodebuild -workspace "${SOLUTION_DIR}/Mengine_Xcode_MacOS.xcworkspace" \
    -scheme MacOSApplication \
    -configuration "${CONFIGURATION}" \
    -clonedSourcePackagesDirPath "${SOLUTION_DIR}/SourcePackages" \
    build
