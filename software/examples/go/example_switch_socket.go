package main

import (
	"fmt"
	"tinkerforge/ipconnection"
	"tinkerforge/remote_switch_v2_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Remote Switch Bricklet 2.0.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	rs, _ := remote_switch_v2_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Switch on a type A socket with house code 17 and receiver code 1.
	// House code 17 is 10001 in binary (least-significant bit first)
	// and means that the DIP switches 1 and 5 are on and 2-4 are off.
	// Receiver code 1 is 10000 in binary (least-significant bit first)
	// and means that the DIP switch A is on and B-E are off.
	rs.SwitchSocketA(17, 1, remote_switch_v2_bricklet.SwitchToOn)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
