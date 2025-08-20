#!/bin/bash

# Start ./MissileCreator in the background
./MissileCreator &
FED_PID=$!

# Wait for a few seconds
sleep 5

# Shut down ./MissileCreator
kill $FED_PID

# Optional: Wait for process to terminate
wait $FED_PID 2>/dev/null

echo "MissileTest started and stopped successfully."