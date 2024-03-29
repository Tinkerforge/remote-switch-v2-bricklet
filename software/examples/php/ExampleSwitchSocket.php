<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRemoteSwitchV2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRemoteSwitchV2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Remote Switch Bricklet 2.0

$ipcon = new IPConnection(); // Create IP connection
$rs = new BrickletRemoteSwitchV2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Switch on a type A socket with house code 17 and receiver code 1.
// House code 17 is 10001 in binary (least-significant bit first)
// and means that the DIP switches 1 and 5 are on and 2-4 are off.
// Receiver code 1 is 10000 in binary (least-significant bit first)
// and means that the DIP switch A is on and B-E are off.
$rs->switchSocketA(17, 1, BrickletRemoteSwitchV2::SWITCH_TO_ON);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
