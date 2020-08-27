#include "bindings/hal_common.h"
#include "bindings/bricklet_remote_switch_v2.h"

#define UID "XYZ" // Change XYZ to the UID of your Remote Switch Bricklet 2.0

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


static TF_RemoteSwitchV2 rs;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_remote_switch_v2_create(&rs, UID, hal), "create device object");

	// Switch on a type A socket with house code 17 and receiver code 1.
	// House code 17 is 10001 in binary (least-significant bit first)
	// and means that the DIP switches 1 and 5 are on and 2-4 are off.
	// Receiver code 1 is 10000 in binary (least-significant bit first)
	// and means that the DIP switch A is on and B-E are off.
	check(tf_remote_switch_v2_switch_socket_a(&rs, 17, 1,
	                                          TF_REMOTE_SWITCH_V2_SWITCH_TO_ON), "call switch_socket_a");
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
