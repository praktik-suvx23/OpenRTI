#!/bin/bash
set -e  # Exit immediately if any command fails

# Resolve the absolute path to the directory containing this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
LOG_DIR="$SCRIPT_DIR/log/AdminFederateTest"

mkdir -p "$LOG_DIR"

INPUT_FILE="$SCRIPT_DIR/input/dummy_input_admin.txt"
OUTPUT_FILE="$LOG_DIR/admin_output.log"

# Clear the log
> "$OUTPUT_FILE"

# Run AdminFederate with dummy input and log its output
cat "$INPUT_FILE" | "$BUILD_DIR/AdminFederate" > "$OUTPUT_FILE" 2>&1 &
PID=$!

sleep 5

# Terminate the AdminFederate process
kill $PID || true
pkill -f AdminFederate || true

sleep 2

# Look for known connection failure patterns in the log
if grep -q -E "rti1516e::ConnectionFailed|rti1516e::NotConnected|\[ERROR\] Unknown Exception in connectToRTI!" "$OUTPUT_FILE"; then
  echo "AdminFederate failed to connect to RTI."
  exit 1
fi

# Check for successful operation indicator in the log
if grep -q "Sent SetupSimulation interaction" "$OUTPUT_FILE"; then
  echo "AdminFederate test passed."
  exit 0
else
  echo "Expected output not found in AdminFederate log."
  exit 1
fi