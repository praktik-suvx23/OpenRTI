#!/bin/bash
SCRIPT_DIR="$(dirname "$0")"

ADMIN_INPUT_FILE_SHORTCUT="$SCRIPT_DIR/dummy_input_admin.txt"

./Ship < "$SCRIPT_DIR/dummy_input_ship1.txt"
sleep 1
./Ship < "$SCRIPT_DIR/dummy_input_ship2.txt"
sleep 1
./MissileCreator
sleep 1
./AdminFederate < "$ADMIN_INPUT_FILE_SHORTCUT"

sleep 5

