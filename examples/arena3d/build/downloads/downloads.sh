#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

cmake -S "$PROJECT_DIR/cmake/downloads" -B "$PROJECT_DIR/solutions/downloads"
cmake --build "$PROJECT_DIR/solutions/downloads"
