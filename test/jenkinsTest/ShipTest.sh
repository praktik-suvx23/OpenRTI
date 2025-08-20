#!/bin/bash

# Start ./Ship in the background
./Ship &
FED_PID=$!

# Wait for a few seconds
sleep 5

# Shut down ./Ship
kill $FED_PID

# Optional: Wait for process to terminate
wait $FED_PID 2>/dev/null

echo "ShipTest started and stopped successfully."