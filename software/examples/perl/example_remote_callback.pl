#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRemoteSwitchV2;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Remote Switch Bricklet 2.0

# Callback subroutine for remote status a callback
sub cb_remote_status_a
{
    my ($house_code, $receiver_code, $switch_to, $repeats) = @_;

    print "House Code: $house_code\n";
    print "Receiver Code: $receiver_code\n";

    if ($switch_to == Tinkerforge::BrickletRemoteSwitchV2->SWITCH_TO_OFF)
    {
        print "Switch To: Off\n";
    }
    elsif ($switch_to == Tinkerforge::BrickletRemoteSwitchV2->SWITCH_TO_ON)
    {
        print "Switch To: On\n";
    }

    print "Repeats: $repeats\n";
    print "\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs = Tinkerforge::BrickletRemoteSwitchV2->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Configure to receive from remote type A with minimum repeats set to 1 and enable callback
$rs->set_remote_configuration($rs->REMOTE_TYPE_A, 1, 1);

# Register remote status a callback to subroutine cb_remote_status_a
$rs->register_callback($rs->CALLBACK_REMOTE_STATUS_A, 'cb_remote_status_a');

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
