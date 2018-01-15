function matlab_example_remote_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletRemoteSwitchV2;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Remote Switch Bricklet 2.0

    ipcon = IPConnection(); % Create IP connection
    rs = handle(BrickletRemoteSwitchV2(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Configure to receive from remote type A with minimum repeats set to 1 and enable callback
    rs.setRemoteConfiguration(BrickletRemoteSwitchV2.REMOTE_TYPE_A, 1, true);

    % Register remote status a callback to function cb_remote_status_a
    set(rs, 'RemoteStatusACallback', @(h, e) cb_remote_status_a(e));

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for remote status a callback
function cb_remote_status_a(e)
    fprintf('House Code: %i\n', e.houseCode);
    fprintf('Receiver Code: %i\n', e.receiverCode);
    fprintf('Switch To: %i\n', e.switchTo);
    fprintf('Repeats: %i\n', e.repeats);
    fprintf('\n');
end
