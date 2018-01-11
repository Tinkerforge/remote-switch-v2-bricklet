function octave_example_remote_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Remote Switch Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    rs = javaObject("com.tinkerforge.BrickletRemoteSwitchV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register remote status a callback to function cb_remote_status_a
    rs.addRemoteStatusACallback(@cb_remote_status_a);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for remote status a callback
function cb_remote_status_a(e)
    fprintf("House Code: %d\n", e.houseCode);
    fprintf("Receiver Code: %d\n", e.receiverCode);
    fprintf("Switch To: %d\n", e.switchTo);
    fprintf("Repeats: %d\n", e.repeats);
    fprintf("\n");
end
