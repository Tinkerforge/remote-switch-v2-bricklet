/* remote-switch-v2-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"

#include "rfm69.h"

extern RFM69 rfm69;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_SWITCHING_STATE: return get_switching_state(message, response);
		case FID_SET_REPEATS: return set_repeats(message);
		case FID_GET_REPEATS: return get_repeats(message, response);
		case FID_SWITCH_SOCKET_A: return switch_socket_a(message);
		case FID_SWITCH_SOCKET_B: return switch_socket_b(message);
		case FID_DIM_SOCKET_B: return dim_socket_b(message);
		case FID_SWITCH_SOCKET_C: return switch_socket_c(message);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse get_switching_state(const GetSwitchingState *data, GetSwitchingState_Response *response) {
	response->header.length = sizeof(GetSwitchingState_Response);
	response->state = rfm69.switching_state;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_repeats(const SetRepeats *data) {
	rfm69.switch_repeats = data->repeats;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_repeats(const GetRepeats *data, GetRepeats_Response *response) {
	response->header.length = sizeof(GetRepeats_Response);
	response->repeats = rfm69.switch_repeats;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse switch_socket_a(const SwitchSocketA *data) {
	if(rfm69.switching_state != REMOTE_SWITCH_V2_SWITCHING_STATE_READY) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rfm69.switch_type_new = TYPE_A_C;

	const uint8_t house_code = data->house_code;
	for(uint8_t i = 0; i < 5; i++) {
		if(house_code & (1 << i)) {
			rfm69.data_switch[i] = RFM69_DATA_ON;
		} else {
			rfm69.data_switch[i] = RFM69_DATA_FLOAT;
		}
	}

	const uint8_t receiver_code = data->receiver_code;
	for(uint8_t i = 0; i < 5; i++) {
		if(receiver_code & (1 << i)) {
			rfm69.data_switch[i+5] = RFM69_DATA_ON;
		} else {
			rfm69.data_switch[i+5] = RFM69_DATA_FLOAT;
		}
	}

	if(data->switch_to == RFM69_SWITCH_TO_ON) {
		rfm69.data_switch[10] = RFM69_DATA_ON;
		rfm69.data_switch[11] = RFM69_DATA_FLOAT;
	} else {
		rfm69.data_switch[10] = RFM69_DATA_FLOAT;
		rfm69.data_switch[11] = RFM69_DATA_ON;
	}

	// Sync
	rfm69.data_switch[12] = 0b10000000;
	rfm69.data_switch[13] = 0b00000000;
	rfm69.data_switch[14] = 0b00000000;
	rfm69.data_switch[15] = 0b00000000;

	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse switch_socket_b(const SwitchSocketB *data) {
	if(rfm69.switching_state != REMOTE_SWITCH_V2_SWITCHING_STATE_READY) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rfm69.switch_type_new = TYPE_B;

	// Sync
	rfm69.data_switch[0] = 0b00000100;
	rfm69.data_switch[1] = 0b00000000;

	// Address
	for(uint8_t i = 0; i < 26; i++) {
		if(data->address & (1 << i)) {
			rfm69.data_switch[i+2] = RFM69_DATA_B_1;
		} else {
			rfm69.data_switch[i+2] = RFM69_DATA_B_0;
		}
	}

	// Switch All
	if(data->unit == 255) {
		rfm69.data_switch[28] = RFM69_DATA_B_1;
	} else {
		rfm69.data_switch[28] = RFM69_DATA_B_0;
	}

	// On/Off
	if(data->switch_to == RFM69_SWITCH_TO_ON) {
		rfm69.data_switch[29] = RFM69_DATA_B_1;
	} else {
		rfm69.data_switch[29] = RFM69_DATA_B_0;
	}

	// Unit
	for(uint8_t i = 0; i < 4; i++) {
		if(data->unit & (1 << i)) {
			rfm69.data_switch[i+30] = RFM69_DATA_B_1;
		} else {
			rfm69.data_switch[i+30] = RFM69_DATA_B_0;
		}
	}

	// Sync
	rfm69.data_switch[34] = 0b10000000;
	rfm69.data_switch[35] = 0b00000000;
	rfm69.data_switch[36] = 0b00000000;
	rfm69.data_switch[37] = 0b00000000;
	rfm69.data_switch[38] = 0b00000000;

	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse dim_socket_b(const DimSocketB *data) {
	if(rfm69.switching_state != REMOTE_SWITCH_V2_SWITCHING_STATE_READY) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rfm69.switch_type_new = TYPE_B_DIM;

	// Sync
	rfm69.data_switch[0] = 0b00000100;
	rfm69.data_switch[1] = 0b00000000;

	// Address
	for(uint8_t i = 0; i < 26; i++) {
		if(data->address & (1 << i)) {
			rfm69.data_switch[i+2] = RFM69_DATA_B_1;
		} else {
			rfm69.data_switch[i+2] = RFM69_DATA_B_0;
		}
	}

	// Switch All
	if(data->unit == 255) {
		rfm69.data_switch[28] = RFM69_DATA_B_1;
	} else {
		rfm69.data_switch[28] = RFM69_DATA_B_0;
	}

	// Dim bit (we have to shift everything by 4 to the left after here
	rfm69.data_switch[29] = 0b10100000;

	// Unit
	for(uint8_t i = 0; i < 4; i++) {
		if(data->unit & (1 << i)) {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_1, rfm69.data_switch, i+30);
		} else {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_0, rfm69.data_switch, i+30);
		}
	}

	// Dim value
	for(uint8_t i = 0; i < 4; i++) {
		if(data->dim_value & (1 << (3-i))) {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_1, rfm69.data_switch, i+34);
		} else {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_0, rfm69.data_switch, i+34);
		}
	}

	// Sync
	rfm69.data_switch[37] |= 0b0001000;
	rfm69.data_switch[38] = 0b00000000;
	rfm69.data_switch[39] = 0b00000000;
	rfm69.data_switch[40] = 0b00000000;
	rfm69.data_switch[41] = 0b00000000;
	rfm69.data_switch[42] = 0b00000000;

	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse switch_socket_c(const SwitchSocketC *data) {
	if(rfm69.switching_state != REMOTE_SWITCH_V2_SWITCHING_STATE_READY) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	uint8_t system_code;
	if(data->system_code >= 'a' && data->system_code <= 'p') {
		system_code = data->system_code - 'a';
	} else if(data->system_code >= 'A' && data->system_code <= 'P') {
		system_code = data->system_code - 'A';
	} else {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->device_code < 1 || data->device_code > 16) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	rfm69.switch_type_new = TYPE_A_C;

	// System Code
	for(uint8_t i = 0; i < 4; i++) {
		if(system_code & (1 << i)) {
			rfm69.data_switch[i] = RFM69_DATA_FLOAT;
		} else {
			rfm69.data_switch[i] = RFM69_DATA_ON;
		}
	}

	// Device Code
	const uint8_t device_code = data->device_code - 1;
	for(uint8_t i = 0; i < 4; i++) {
		if(device_code & (1 << i)) {
			rfm69.data_switch[i+4] = RFM69_DATA_FLOAT;
		} else {
			rfm69.data_switch[i+4] = RFM69_DATA_ON;
		}
	}

	// Fixed On+Float
	rfm69.data_switch[8] = RFM69_DATA_ON;
	rfm69.data_switch[9] = RFM69_DATA_FLOAT;

	// On/Off
	if(data->switch_to == RFM69_SWITCH_TO_ON) {
		rfm69.data_switch[10] = RFM69_DATA_FLOAT;
		rfm69.data_switch[11] = RFM69_DATA_FLOAT;
	} else {
		rfm69.data_switch[10] = RFM69_DATA_FLOAT;
		rfm69.data_switch[11] = RFM69_DATA_ON;
	}

	// Sync
	rfm69.data_switch[12] = 0b10000000;
	rfm69.data_switch[13] = 0b00000000;
	rfm69.data_switch[14] = 0b00000000;
	rfm69.data_switch[15] = 0b00000000;

	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}




bool handle_switching_done_callback(void) {
	static bool is_buffered = false;
	static SwitchingDone_Callback cb;

	if(!is_buffered) {
		if(rfm69.switching_done_send_callback) {
			rfm69.switching_done_send_callback =  false;
			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(SwitchingDone_Callback), FID_CALLBACK_SWITCHING_DONE);
		} else {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(SwitchingDone_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	communication_callback_init();
}
