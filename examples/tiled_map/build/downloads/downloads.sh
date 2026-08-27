#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SOLUTION_DIR="$SCRIPT_DIR/../../solutions/downloads"
CMAKE_SOURCE_DIR="$SCRIPT_DIR/../../cmake/Downloads"

mkdir -p "$SOLUTION_DIR"
cmake -S "$CMAKE_SOURCE_DIR" -B "$SOLUTION_DIR"
cmake --build "$SOLUTION_DIR"
