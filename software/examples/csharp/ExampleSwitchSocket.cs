using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Remote Switch Bricklet 2.0

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRemoteSwitchV2 rs = new BrickletRemoteSwitchV2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Switch on a type A socket with house code 17 and receiver code 1.
		// House code 17 is 10001 in binary (least-significant bit first)
		// and means that the DIP switches 1 and 5 are on and 2-4 are off.
		// Receiver code 1 is 10000 in binary (least-significant bit first)
		// and means that the DIP switch A is on and B-E are off.
		rs.SwitchSocketA(17, 1, BrickletRemoteSwitchV2.SWITCH_TO_ON);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
