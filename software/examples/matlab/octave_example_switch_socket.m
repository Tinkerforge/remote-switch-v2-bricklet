function octave_example_switch_socket()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Remote Switch Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    rs = javaObject("com.tinkerforge.BrickletRemoteSwitchV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Switch on a type A socket with house code 17 and receiver code 1.
    % House code 17 is 10001 in binary (least-significant bit first)
    % and means that the DIP switches 1 and 5 are on and 2-4 are off.
    % Receiver code 1 is 10000 in binary (least-significant bit first)
    % and means that the DIP switch A is on and B-E are off.
    rs.switchSocketA(17, 1, rs.SWITCH_TO_ON);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
