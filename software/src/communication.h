/* remote-switch-v2-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants
#define REMOTE_SWITCH_V2_SWITCHING_STATE_READY 0
#define REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY 1

#define REMOTE_SWITCH_V2_SWITCH_TO_OFF 0
#define REMOTE_SWITCH_V2_SWITCH_TO_ON 1

#define REMOTE_SWITCH_V2_REMOTE_TYPE_A 0
#define REMOTE_SWITCH_V2_REMOTE_TYPE_B 1
#define REMOTE_SWITCH_V2_REMOTE_TYPE_C 2

#define REMOTE_SWITCH_V2_BOOTLOADER_MODE_BOOTLOADER 0
#define REMOTE_SWITCH_V2_BOOTLOADER_MODE_FIRMWARE 1
#define REMOTE_SWITCH_V2_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define REMOTE_SWITCH_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define REMOTE_SWITCH_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define REMOTE_SWITCH_V2_BOOTLOADER_STATUS_OK 0
#define REMOTE_SWITCH_V2_BOOTLOADER_STATUS_INVALID_MODE 1
#define REMOTE_SWITCH_V2_BOOTLOADER_STATUS_NO_CHANGE 2
#define REMOTE_SWITCH_V2_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define REMOTE_SWITCH_V2_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define REMOTE_SWITCH_V2_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define REMOTE_SWITCH_V2_STATUS_LED_CONFIG_OFF 0
#define REMOTE_SWITCH_V2_STATUS_LED_CONFIG_ON 1
#define REMOTE_SWITCH_V2_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define REMOTE_SWITCH_V2_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_GET_SWITCHING_STATE 1
#define FID_SET_REPEATS 3
#define FID_GET_REPEATS 4
#define FID_SWITCH_SOCKET_A 5
#define FID_SWITCH_SOCKET_B 6
#define FID_DIM_SOCKET_B 7
#define FID_SWITCH_SOCKET_C 8
#define FID_SET_REMOTE_CONFIGURATION 9
#define FID_GET_REMOTE_CONFIGURATION 10
#define FID_GET_REMOTE_STATUS_A 11
#define FID_GET_REMOTE_STATUS_B 12
#define FID_GET_REMOTE_STATUS_C 13

#define FID_CALLBACK_SWITCHING_DONE 2
#define FID_CALLBACK_REMOTE_STATUS_A 14
#define FID_CALLBACK_REMOTE_STATUS_B 15
#define FID_CALLBACK_REMOTE_STATUS_C 16

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetSwitchingState;

typedef struct {
	TFPMessageHeader header;
	uint8_t state;
} __attribute__((__packed__)) GetSwitchingState_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) SwitchingDone_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t repeats;
} __attribute__((__packed__)) SetRepeats;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetRepeats;

typedef struct {
	TFPMessageHeader header;
	uint8_t repeats;
} __attribute__((__packed__)) GetRepeats_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t house_code;
	uint8_t receiver_code;
	uint8_t switch_to;
} __attribute__((__packed__)) SwitchSocketA;

typedef struct {
	TFPMessageHeader header;
	uint32_t address;
	uint8_t unit;
	uint8_t switch_to;
} __attribute__((__packed__)) SwitchSocketB;

typedef struct {
	TFPMessageHeader header;
	uint32_t address;
	uint8_t unit;
	uint8_t dim_value;
} __attribute__((__packed__)) DimSocketB;

typedef struct {
	TFPMessageHeader header;
	char system_code;
	uint8_t device_code;
	uint8_t switch_to;
} __attribute__((__packed__)) SwitchSocketC;

typedef struct {
	TFPMessageHeader header;
	uint8_t remote_type;
	uint16_t minimum_repeats;
	bool callback_enabled;
} __attribute__((__packed__)) SetRemoteConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetRemoteConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t remote_type;
	uint16_t minimum_repeats;
	bool callback_enabled;
} __attribute__((__packed__)) GetRemoteConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetRemoteStatusA;

typedef struct {
	TFPMessageHeader header;
	uint8_t house_code;
	uint8_t receiver_code;
	uint8_t switch_to;
	uint16_t repeats;
} __attribute__((__packed__)) GetRemoteStatusA_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetRemoteStatusB;

typedef struct {
	TFPMessageHeader header;
	uint32_t address;
	uint8_t unit;
	uint8_t switch_to;
	uint8_t dim_value;
	uint16_t repeats;
} __attribute__((__packed__)) GetRemoteStatusB_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetRemoteStatusC;

typedef struct {
	TFPMessageHeader header;
	char system_code;
	uint8_t device_code;
	uint8_t switch_to;
	uint16_t repeats;
} __attribute__((__packed__)) GetRemoteStatusC_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t house_code;
	uint8_t receiver_code;
	uint8_t switch_to;
	uint16_t repeats;
} __attribute__((__packed__)) RemoteStatusA_Callback;

typedef struct {
	TFPMessageHeader header;
	uint32_t address;
	uint8_t unit;
	uint8_t switch_to;
	uint8_t dim_value;
	uint16_t repeats;
} __attribute__((__packed__)) RemoteStatusB_Callback;

typedef struct {
	TFPMessageHeader header;
	char system_code;
	uint8_t device_code;
	uint8_t switch_to;
	uint16_t repeats;
} __attribute__((__packed__)) RemoteStatusC_Callback;


// Function prototypes
BootloaderHandleMessageResponse get_switching_state(const GetSwitchingState *data, GetSwitchingState_Response *response);
BootloaderHandleMessageResponse set_repeats(const SetRepeats *data);
BootloaderHandleMessageResponse get_repeats(const GetRepeats *data, GetRepeats_Response *response);
BootloaderHandleMessageResponse switch_socket_a(const SwitchSocketA *data);
BootloaderHandleMessageResponse switch_socket_b(const SwitchSocketB *data);
BootloaderHandleMessageResponse dim_socket_b(const DimSocketB *data);
BootloaderHandleMessageResponse switch_socket_c(const SwitchSocketC *data);
BootloaderHandleMessageResponse set_remote_configuration(const SetRemoteConfiguration *data);
BootloaderHandleMessageResponse get_remote_configuration(const GetRemoteConfiguration *data, GetRemoteConfiguration_Response *response);
BootloaderHandleMessageResponse get_remote_status_a(const GetRemoteStatusA *data, GetRemoteStatusA_Response *response);
BootloaderHandleMessageResponse get_remote_status_b(const GetRemoteStatusB *data, GetRemoteStatusB_Response *response);
BootloaderHandleMessageResponse get_remote_status_c(const GetRemoteStatusC *data, GetRemoteStatusC_Response *response);

// Callbacks
bool handle_switching_done_callback(void);
bool handle_remote_status_a_callback(void);
bool handle_remote_status_b_callback(void);
bool handle_remote_status_c_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 4
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_switching_done_callback, \
	handle_remote_status_a_callback, \
	handle_remote_status_b_callback, \
	handle_remote_status_c_callback, \


#endif
