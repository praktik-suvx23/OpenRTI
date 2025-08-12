#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
LOG_DIR="$SCRIPT_DIR/log/AdminFederateTest"

mkdir -p "$LOG_DIR"

INPUT_FILE="$SCRIPT_DIR/dummy_input_admin.txt"
OUTPUT_FILE="$LOG_DIR/admin_output.log"

> "$OUTPUT_FILE"

cat "$INPUT_FILE" | "$BUILD_DIR/AdminFederate" > "$OUTPUT_FILE" 2>&1 &
PID=$!

sleep 5

# Kill the AdminFederate process by PID
kill $PID || true

# Fallback: kill any lingering AdminFederate processes by name
pkill -f AdminFederate || true

# Optional: wait a moment for ports to be released
sleep 2

# Check for connection errors or unknown exceptions in admin log
if grep -q -E "rti1516e::ConnectionFailed|rti1516e::NotConnected|\[ERROR\] Unknown Exception in connectToRTI!" "$OUTPUT_FILE"; then
  echo "AdminFederate failed to connect to RTI."
  exit 1
fi

if grep -q "Sent SetupSimulation interaction" "$OUTPUT_FILE"; then
  echo "AdminFederate test passed."
  exit 0
else
  echo "Expected output not found in AdminFederate log."
  exit 1
fi