package main

import (
	"fmt"
	"github.com/tinkerforge/go-api-bindings/ipconnection"
	"github.com/tinkerforge/go-api-bindings/remote_switch_v2_bricklet"
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

	// Configure to receive from remote type A with minimum repeats set to 1 and enable callback
	rs.SetRemoteConfiguration(remote_switch_v2_bricklet.RemoteTypeA, 1, true)

	rs.RegisterRemoteStatusACallback(func(houseCode uint8, receiverCode uint8, switchTo remote_switch_v2_bricklet.SwitchTo, repeats uint16) {
		fmt.Printf("House Code: %d\n", houseCode)
		fmt.Printf("Receiver Code: %d\n", receiverCode)

		if switchTo == remote_switch_v2_bricklet.SwitchToOff {
			fmt.Println("Switch To: Off")
		} else if switchTo == remote_switch_v2_bricklet.SwitchToOn {
			fmt.Println("Switch To: On")
		}

		fmt.Printf("Repeats: %d\n", repeats)
		fmt.Println()
	})

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
