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

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_repeats(const SetRepeats *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_repeats(const GetRepeats *data, GetRepeats_Response *response) {
	response->header.length = sizeof(GetRepeats_Response);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse switch_socket_a(const SwitchSocketA *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse switch_socket_b(const SwitchSocketB *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse dim_socket_b(const DimSocketB *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse switch_socket_c(const SwitchSocketC *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}




bool handle_switching_done_callback(void) {
	static bool is_buffered = false;
	static SwitchingDone_Callback cb;

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(SwitchingDone_Callback), FID_CALLBACK_SWITCHING_DONE);
		// TODO: Implement SwitchingDone callback handling

		return false;
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
