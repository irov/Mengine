#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
CONFIGURATION=${1:-Debug}
SOLUTION_DIR=$PROJECT_DIR/solutions/headless/$CONFIGURATION

cmake -S "$PROJECT_DIR" -B "$SOLUTION_DIR" -DCMAKE_BUILD_TYPE="$CONFIGURATION"
cmake --build "$SOLUTION_DIR" --parallel
