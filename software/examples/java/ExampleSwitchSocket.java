import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRemoteSwitchV2;

public class ExampleSwitchSocket {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your Remote Switch Bricklet 2.0
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRemoteSwitchV2 rs = new BrickletRemoteSwitchV2(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Switch on a type A socket with house code 17 and receiver code 1.
		// House code 17 is 10001 in binary (least-significant bit first)
		// and means that the DIP switches 1 and 5 are on and 2-4 are off.
		// Receiver code 1 is 10000 in binary (least-significant bit first)
		// and means that the DIP switch A is on and B-E are off.
		rs.switchSocketA(17, 1, BrickletRemoteSwitchV2.SWITCH_TO_ON);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
