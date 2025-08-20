#!/bin/bash

# Start ./AdminFederate in the background
./AdminFederate &
FED_PID=$!

# Wait for a few seconds
sleep 5

# Shut down ./AdminFederate
kill $FED_PID

# Optional: Wait for process to terminate
wait $FED_PID 2>/dev/null

echo "AdminFederate started and stopped successfully."