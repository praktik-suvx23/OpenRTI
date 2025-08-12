#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)" 
BUILD_DIR="$PROJECT_ROOT/build"
INPUT_DIR="$SCRIPT_DIR/inputs"
LOG_DIR="$SCRIPT_DIR/logs"

mkdir -p "$LOG_DIR"

cd /usr/OjOpenRTI/build
#Needs to start first otherwise it wont work
gnome-terminal -- bash -c "$BUILD_DIR/AdminFederate; exec bash" 
sleep 1
gnome-terminal -- bash -c "$BUILD_DIR/MissileCreator; exec bash"
sleep 1
gnome-terminal -- bash -c "cat $INPUT_DIR/Ship1.txt | $BUILD_DIR/Ship; exec bash"
sleep 1
gnome-terminal -- bash -c "cat $INPUT_DIR/Ship2.txt | $BUILD_DIR/Ship; exec bash"
sleep 5
