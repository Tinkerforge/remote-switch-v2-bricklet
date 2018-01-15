var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your Remote Switch Bricklet 2.0

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var rs = new Tinkerforge.BrickletRemoteSwitchV2(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Configure to receive from remote type A with minimum repeats set to 1 and enable callback
        rs.setRemoteConfiguration(Tinkerforge.BrickletRemoteSwitchV2.REMOTE_TYPE_A, 1,
                                  true);
    }
);

// Register remote status a callback
rs.on(Tinkerforge.BrickletRemoteSwitchV2.CALLBACK_REMOTE_STATUS_A,
    // Callback function for remote status a callback
    function (houseCode, receiverCode, switchTo, repeats) {
        console.log('House Code: ' + houseCode);
        console.log('Receiver Code: ' + receiverCode);
        console.log('Switch To: ' + switchTo);
        console.log('Repeats: ' + repeats);
        console.log();
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
