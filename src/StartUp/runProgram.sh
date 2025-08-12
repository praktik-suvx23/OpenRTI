#!/bin/bash

#Working sometimes (make sure that admin terminal is in focus)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
INPUT_DIR="$SCRIPT_DIR/inputs"
LOG_DIR="$SCRIPT_DIR/logs"
PYTHON_DIR="$PROJECT_ROOT/src/VisualRepresentation"

mkdir -p "$LOG_DIR"

cd "$BUILD_DIR"

gnome-terminal -- bash -c "cat $PYTHON_DIR/ReceiveData.py | python3; exit"
sleep 1
gnome-terminal -- bash -c "./PyLink; exit"
sleep 1
gnome-terminal -- bash -c "./AdminFederate; exit"
sleep 1
gnome-terminal -- bash -c "./MissileCreator; exit"
sleep 1
gnome-terminal -- bash -c "cat $INPUT_DIR/Ship1.txt | ./Ship; exit"
sleep 1
gnome-terminal -- bash -c "cat $INPUT_DIR/Ship2.txt | ./Ship; exit"
sleep 2

# Now feed AdminFederate its input by writing to the pipe
sleep 30

pkill -f Ship
pkill -f MissileCreator
pkill -f AdminFederate
pkill -f PyLink

