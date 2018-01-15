using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Remote Switch Bricklet 2.0

	// Callback function for remote status a callback
	static void RemoteStatusACB(BrickletRemoteSwitchV2 sender, byte houseCode,
	                            byte receiverCode, byte switchTo, int repeats)
	{
		Console.WriteLine("House Code: " + houseCode);
		Console.WriteLine("Receiver Code: " + receiverCode);
		Console.WriteLine("Switch To: " + switchTo);
		Console.WriteLine("Repeats: " + repeats);
		Console.WriteLine("");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRemoteSwitchV2 rs = new BrickletRemoteSwitchV2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Configure to receive from remote type A with minimum repeats set to 1 and enable callback
		rs.SetRemoteConfiguration(BrickletRemoteSwitchV2.REMOTE_TYPE_A, 1, true);

		// Register remote status a callback to function RemoteStatusACB
		rs.RemoteStatusACallback += RemoteStatusACB;

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
