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
		case FID_SET_REMOTE_CONFIGURATION: return set_remote_configuration(message);
		case FID_GET_REMOTE_CONFIGURATION: return get_remote_configuration(message, response);
		case FID_GET_REMOTE_STATUS_A: return get_remote_status_a(message, response);
		case FID_GET_REMOTE_STATUS_B: return get_remote_status_b(message, response);
		case FID_GET_REMOTE_STATUS_C: return get_remote_status_c(message, response);
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

BootloaderHandleMessageResponse set_remote_configuration(const SetRemoteConfiguration *data) {
	if(data->remote_type > REMOTE_SWITCH_V2_REMOTE_TYPE_C || data->minimum_repeats == 0) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rfm69.remote_type != data->remote_type) {
		rfm69.remote_update = true; // reconfigure receiver with new bitrate if type changes
	}

	rfm69.remote_type             = data->remote_type;
	rfm69.remote_minimum_repeats  = data->minimum_repeats;
	rfm69.remote_callback_enabled = data->callback_enabled;

	switch(rfm69.remote_type) {
		case REMOTE_SWITCH_V2_REMOTE_TYPE_A: rfm69.data_receive_command_length = TYPE_A_C_PACKET_LENGTH_RECEIVE; break;
		case REMOTE_SWITCH_V2_REMOTE_TYPE_B: rfm69.data_receive_command_length = TYPE_B_PACKET_LENGTH_RECEIVE; break;
		case REMOTE_SWITCH_V2_REMOTE_TYPE_C: rfm69.data_receive_command_length = TYPE_A_C_PACKET_LENGTH_RECEIVE; break;
		default: break;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_remote_configuration(const GetRemoteConfiguration *data, GetRemoteConfiguration_Response *response) {
	response->header.length    = sizeof(GetRemoteConfiguration_Response);
	response->remote_type      = rfm69.remote_type;
	response->minimum_repeats  = rfm69.remote_minimum_repeats;
	response->callback_enabled = rfm69.remote_callback_enabled;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_remote_status_a(const GetRemoteStatusA *data, GetRemoteStatusA_Response *response) {
	response->header.length     = sizeof(GetRemoteStatusA_Response);

	response->repeats           = rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_A];

	if(response->repeats == 0 || rfm69.remote_type != REMOTE_SWITCH_V2_REMOTE_TYPE_A) {
		response->house_code    = 0;
		response->receiver_code = 0;
		response->switch_to     = 0;
	} else {
		response->house_code    = rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_A] & 0b11111;
		response->receiver_code = (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_A] >> 5) & 0b11111;
		response->switch_to     = (((rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_A] >> 10) & 0b11) == 0b10) ? 0 : 1;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_remote_status_b(const GetRemoteStatusB *data, GetRemoteStatusB_Response *response) {
	response->header.length = sizeof(GetRemoteStatusB_Response);

	response->repeats       = rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_B];

	uint64_t command = rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B];
	uint32_t command1 = command & 0xFFFFFFFF;
	uint32_t command2 = (command >> 32L) & 0xFFFFFFFF;

	if(response->repeats == 0 || rfm69.remote_type != REMOTE_SWITCH_V2_REMOTE_TYPE_B) {
		response->address   = 0;
		response->unit      = 0;
		response->switch_to = 0;
		response->dim_value = 0;
	} else {
		response->address   = (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B] >> 2) & 0b11111111111111111111111111;;
		response->unit      = (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B] >> 30) & 0b1111;
		if((rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B] >> 28) & 0b1) {
			response->unit  = 0xff;
		}
		response->switch_to = (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B] >> 29) & 0b1;
		response->dim_value = 0;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_remote_status_c(const GetRemoteStatusC *data, GetRemoteStatusC_Response *response) {
	response->header.length = sizeof(GetRemoteStatusC_Response);

	response->repeats         = rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_C];

	if(response->repeats == 0 || rfm69.remote_type != REMOTE_SWITCH_V2_REMOTE_TYPE_C) {
		response->system_code = 'A';
		response->device_code = 0;
		response->switch_to   = 0;
	} else {
		response->system_code = 'A' + ((~rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_C]) & 0b1111);
		response->device_code = 1 + (((~rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_C]) >> 4) & 0b1111);
		response->switch_to   = ((((~rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_C]) >> 10) & 0b11) == 0b01) ? 0 : 1;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
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

uint32_t  tt = 0;
bool handle_remote_status_a_callback(void) {
	static bool is_buffered = false;
	static RemoteStatusA_Callback cb;

	static uint64_t command = 0;
	static uint32_t count   = 0;

	if(!is_buffered) {
		if(rfm69.remote_callback_enabled &&
		   (rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_A] >= rfm69.remote_minimum_repeats) &&
		   ((rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_A] != count) ||
		    (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_A] != command))) {

			command = rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_A];
			count = rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_A];

			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(RemoteStatusA_Callback), FID_CALLBACK_REMOTE_STATUS_A);
			cb.repeats           = count;

			if(count == 0) {
				cb.house_code    = 0;
				cb.receiver_code = 0;
				cb.switch_to     = 0;
			} else {
				cb.house_code    = command & 0b11111;
				cb.receiver_code = (command >> 5) & 0b11111;
				cb.switch_to     = (((command >> 10) & 0b11) == 0b10) ? 0 : 1;
			}
		} else {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(RemoteStatusA_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

bool handle_remote_status_b_callback(void) {
	static bool is_buffered = false;
	static RemoteStatusB_Callback cb;

	static uint64_t command = 0;
	static uint32_t count   = 0;

	if(!is_buffered) {
		if(rfm69.remote_callback_enabled &&
		   (rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_B] >= rfm69.remote_minimum_repeats) &&
		   ((rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_B] != count) ||
		    (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B] != command))) {

			command = rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_B];
			count = rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_B];

			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(RemoteStatusB_Callback), FID_CALLBACK_REMOTE_STATUS_B);
			cb.repeats       = count;

			if(count == 0) {
				cb.address   = 0;
				cb.unit      = 0;
				cb.switch_to = 0;
				cb.dim_value = 0;
			} else {
				cb.address   = (command >> 2) & 0b11111111111111111111111111;;
				cb.unit      = (command >> 30) & 0b1111;
				if((command >> 28) & 0b1) {
					cb.unit  = 0xff;
				}
				cb.switch_to = (command >> 29) & 0b1;
				cb.dim_value = 0;
			}
		} else {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(RemoteStatusB_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

bool handle_remote_status_c_callback(void) {
	static bool is_buffered = false;
	static RemoteStatusC_Callback cb;

	static uint64_t command = 0;
	static uint32_t count   = 0;

	if(!is_buffered) {
		if(rfm69.remote_callback_enabled &&
		   (rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_C] >= rfm69.remote_minimum_repeats) &&
		   ((rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_C] != count) ||
		    (rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_C] != command))) {

			command = rfm69.data_receive_command_last[REMOTE_SWITCH_V2_REMOTE_TYPE_C];
			count = rfm69.data_receive_command_count[REMOTE_SWITCH_V2_REMOTE_TYPE_C];

			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(RemoteStatusC_Callback), FID_CALLBACK_REMOTE_STATUS_C);
			cb.repeats         = count;

			if(cb.repeats == 0) {
				cb.system_code = 'A';
				cb.device_code = 0;
				cb.switch_to   = 0;
			} else {
				cb.system_code = 'A' + ((~command) & 0b1111);
				cb.device_code = 1 + (((~command) >> 4) & 0b1111);
				cb.switch_to   = ((((~command) >> 10) & 0b11) == 0b01) ? 0 : 1;
			}
		} else {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(RemoteStatusC_Callback));
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
