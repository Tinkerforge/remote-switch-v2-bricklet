import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletRemoteSwitchV2;

public class ExampleRemoteCallback {
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

		// Add remote status a listener
		rs.addRemoteStatusAListener(new BrickletRemoteSwitchV2.RemoteStatusAListener() {
			public void remoteStatusA(int houseCode, int receiverCode, int switchTo,
			                          int repeats) {
				System.out.println("House Code: " + houseCode);
				System.out.println("Receiver Code: " + receiverCode);
				System.out.println("Switch To: " + switchTo);
				System.out.println("Repeats: " + repeats);
				System.out.println("");
			}
		});

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
