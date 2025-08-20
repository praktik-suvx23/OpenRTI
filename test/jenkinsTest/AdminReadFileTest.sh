#!/bin/bash


SCRIPT_DIR="$(dirname "$0")"
ADMIN_INPUT_FILE_SHORTCUT="$SCRIPT_DIR/dummy_input_admin.txt"
./admin < "$ADMIN_INPUT_FILE_SHORTCUT"

sleep 5

