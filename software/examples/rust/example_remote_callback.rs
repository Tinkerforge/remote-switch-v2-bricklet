use std::{error::Error, io, thread};
use tinkerforge::{ipconnection::IpConnection, remote_switch_v2_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Remote Switch Bricklet 2.0

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let remote_switch_v2_bricklet = RemoteSwitchV2Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Configure to receive from remote type A with minimum repeats set to 1 and enable callback
    remote_switch_v2_bricklet.set_remote_configuration(REMOTE_SWITCH_V2_BRICKLET_REMOTE_TYPE_A, 1, true);

    //Create listener for remote status a events.
    let remote_status_a_listener = remote_switch_v2_bricklet.get_remote_status_a_receiver();
    // Spawn thread to handle received events. This thread ends when the remote_switch_v2_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in remote_status_a_listener {
            println!("House Code: {}", event.house_code);
            println!("Receiver Code: {}", event.receiver_code);
            println!("Switch To: {}", event.switch_to);
            println!("Repeats: {}", event.repeats);
            println!();
        }
    });

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
