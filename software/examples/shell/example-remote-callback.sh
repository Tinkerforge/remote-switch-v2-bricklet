#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Remote Switch Bricklet 2.0

# Handle incoming remote status a callbacks
tinkerforge dispatch remote-switch-v2-bricklet $uid remote-status-a &

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
