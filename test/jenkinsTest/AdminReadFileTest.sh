#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
# Start admin and provide inputs from the existing text file

./admin < "$SCRIPT_DIR/input/dummy_input_admin.txt"
