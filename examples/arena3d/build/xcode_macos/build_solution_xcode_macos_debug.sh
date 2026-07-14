#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
sh "$SCRIPT_DIR/build_solution_xcode_macos.sh" Debug "$1" "$2" "$3"
