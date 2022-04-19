// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_remote_switch_v2.h"

#define UID "XYZ" // Change XYZ to the UID of your Remote Switch Bricklet 2.0

void check(int rc, const char* msg);

void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);


// Callback function for remote status a callback
static void remote_status_a_handler(TF_RemoteSwitchV2 *device, uint8_t house_code,
                                    uint8_t receiver_code, uint8_t switch_to,
                                    uint16_t repeats, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("House Code: %I8u\n", house_code);
	tf_hal_printf("Receiver Code: %I8u\n", receiver_code);

	if (switch_to == TF_REMOTE_SWITCH_V2_SWITCH_TO_OFF) {
		tf_hal_printf("Switch To: Off\n");
	} else if (switch_to == TF_REMOTE_SWITCH_V2_SWITCH_TO_ON) {
		tf_hal_printf("Switch To: On\n");
	}

	tf_hal_printf("Repeats: %I16u\n", repeats);
	tf_hal_printf("\n");
}

static TF_RemoteSwitchV2 rs;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_remote_switch_v2_create(&rs, UID, hal), "create device object");

	// Configure to receive from remote type A with minimum repeats set to 1 and enable callback
	check(tf_remote_switch_v2_set_remote_configuration(&rs,
	                                                   TF_REMOTE_SWITCH_V2_REMOTE_TYPE_A,
	                                                   1,
	                                                   true), "call set_remote_configuration");

	// Register remote status a callback to function remote_status_a_handler
	tf_remote_switch_v2_register_remote_status_a_callback(&rs,
	                                                      remote_status_a_handler,
	                                                      NULL);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
