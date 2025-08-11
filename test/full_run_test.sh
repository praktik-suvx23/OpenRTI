#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
LOG_DIR="$SCRIPT_DIR/log/FullTest"

mkdir -p "$LOG_DIR"

PYLINK_LOG="$LOG_DIR/pylink_output.log"
ADMIN_LOG="$LOG_DIR/admin_output.log"
SHIP1_LOG="$LOG_DIR/ship1_output.log"
SHIP2_LOG="$LOG_DIR/ship2_output.log"
MISSILE_LOG="$LOG_DIR/missilecreator_output.log"

> "$PYLINK_LOG"
> "$ADMIN_LOG"
> "$SHIP1_LOG"
> "$SHIP2_LOG"
> "$MISSILE_LOG"

"$BUILD_DIR/PyLink" > "$PYLINK_LOG" 2>&1 & PYLINK_PID=$!
"$BUILD_DIR/MissileCreator" > "$MISSILE_LOG" 2>&1 & MISSILE_PID=$!
python3 "$PROJECT_ROOT/src/VisualRepresentation/ReceiveData.py" > /dev/null 2>&1 & PYTHON_PID=$!

sleep 2

cat "$SCRIPT_DIR/dummy_input_ship1.txt" | "$BUILD_DIR/Ship" > "$SHIP1_LOG" 2>&1 & SHIP1_PID=$!
sleep 1
cat "$SCRIPT_DIR/dummy_input_ship2.txt" | "$BUILD_DIR/Ship" > "$SHIP2_LOG" 2>&1 & SHIP2_PID=$!

sleep 3

while IFS= read -r line; do
    echo "$line"
    sleep 0.5
done < "$SCRIPT_DIR/dummy_input_admin.txt" | "$BUILD_DIR/AdminFederate" > "$ADMIN_LOG" 2>&1 & ADMIN_PID=$!

sleep 10

kill $SHIP1_PID $SHIP2_PID $ADMIN_PID $PYLINK_PID $MISSILE_PID $PYTHON_PID || true

echo "Full simulation test complete."
echo "Logs written to:"
echo "   - $PYLINK_LOG"
echo "   - $ADMIN_LOG"
echo "   - $SHIP1_LOG"
echo "   - $SHIP2_LOG"
echo "   - $MISSILE_LOG"