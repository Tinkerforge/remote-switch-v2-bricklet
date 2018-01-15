<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRemoteSwitchV2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRemoteSwitchV2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Remote Switch Bricklet 2.0

// Callback function for remote status a callback
function cb_remoteStatusA($house_code, $receiver_code, $switch_to, $repeats)
{
    echo "House Code: $house_code\n";
    echo "Receiver Code: $receiver_code\n";
    echo "Switch To: $switch_to\n";
    echo "Repeats: $repeats\n";
    echo "\n";
}

$ipcon = new IPConnection(); // Create IP connection
$rs = new BrickletRemoteSwitchV2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Configure to receive from remote type A with minimum repeats set to 1 and enable callback
$rs->setRemoteConfiguration(BrickletRemoteSwitchV2::REMOTE_TYPE_A, 1, TRUE);

// Register remote status a callback to function cb_remoteStatusA
$rs->registerCallback(BrickletRemoteSwitchV2::CALLBACK_REMOTE_STATUS_A,
                      'cb_remoteStatusA');

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
