#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_remote_switch_v2'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Remote Switch Bricklet 2.0

ipcon = IPConnection.new # Create IP connection
rs = BrickletRemoteSwitchV2.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Configure to receive from remote type A with minimum repeats set to 1 and enable callback
rs.set_remote_configuration BrickletRemoteSwitchV2::REMOTE_TYPE_A, 1, true

# Register remote status a callback
rs.register_callback(BrickletRemoteSwitchV2::CALLBACK_REMOTE_STATUS_A) do |house_code,
                                                                           receiver_code,
                                                                           switch_to,
                                                                           repeats|
  puts "House Code: #{house_code}"
  puts "Receiver Code: #{receiver_code}"
  puts "Switch To: #{switch_to}"
  puts "Repeats: #{repeats}"
  puts ''
end

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
