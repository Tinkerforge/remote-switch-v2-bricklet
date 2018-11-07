use std::{error::Error, io, thread};
use tinkerforge::{ip_connection::IpConnection, remote_switch_v2_bricklet::*};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Remote Switch Bricklet 2.0.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let rs = RemoteSwitchV2Bricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Configure to receive from remote type A with minimum repeats set to 1 and enable callback
    rs.set_remote_configuration(REMOTE_SWITCH_V2_BRICKLET_REMOTE_TYPE_A, 1, true);

    // Create receiver for remote status a events.
    let remote_status_a_receiver = rs.get_remote_status_a_receiver();

    // Spawn thread to handle received events. This thread ends when the `rs` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for remote_status_a in remote_status_a_receiver {
            println!("House Code: {}", remote_status_a.house_code);
            println!("Receiver Code: {}", remote_status_a.receiver_code);
            println!("Switch To: {}", remote_status_a.switch_to);
            println!("Repeats: {}", remote_status_a.repeats);
            println!();
        }
    });

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
