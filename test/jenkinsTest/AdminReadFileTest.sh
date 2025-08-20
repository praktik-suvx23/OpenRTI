#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
INPUT_DIR="$SCRIPT_DIR/input"
LOG_DIR="$SCRIPT_DIR/logs"
PYTHON_DIR="$PROJECT_ROOT/src/VisualRepresentation"
# Start admin and provide inputs from the existing text file

./admin < "../test/input/dummy_input_admin.txt"

sleep 5

