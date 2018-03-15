/* remote-switch-v2-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rfm69.c: RFM69 ISM transceiver driver
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

#include "rfm69.h"

#include <string.h>

#include "configs/config_rfm69.h"

#include "bricklib2/logging/logging.h"
#include "bricklib2/os/coop_task.h"
#include "xmc_gpio.h"
#include "xmc_spi.h"

#include "communication.h"

CoopTask rfm69_task;
RFM69 rfm69;

#define rfm69_rx_irq_handler IRQ_Hdlr_11
#define rfm69_tx_irq_handler IRQ_Hdlr_12

const uint8_t rfm69_config_general[][2] = {
	{REG_FRFMSB, RF_FRFMSB_433_92}, // 433.92 Mhz
	{REG_FRFMID, RF_FRFMID_433_92},
	{REG_FRFLSB, RF_FRFLSB_433_92},
	{REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24},
	{REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00}, // Use OOK
	{REG_BITRATEMSB, TYPE_A_C_BITRATEMSB},
	{REG_BITRATELSB, TYPE_A_C_BITRATELSB},
	{REG_PREAMBLEMSB, 0x00}, // no preamble
	{REG_PREAMBLELSB, 0x00},
	{REG_SYNCCONFIG, RF_SYNC_OFF | RF_SYNC_FIFOFILL_MANUAL}, // Use empty sync to assure small pause between commands
	{REG_SYNCVALUE1, 0},
	{REG_SYNCVALUE2, 0},
	{REG_SYNCVALUE3, 0},
	{REG_SYNCVALUE4, 0},
	{REG_SYNCVALUE5, 0},
	{REG_SYNCVALUE6, 0},
	{REG_SYNCVALUE7, 0},
	{REG_SYNCVALUE8, 0},
	{REG_PACKETCONFIG1, RF_PACKET1_FORMAT_FIXED | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF},
	{REG_PAYLOADLENGTH, 0},
	{REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY}, // Set fifo threshold, send when threshold reached
	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111},
	{REG_DIOMAPPING1, 0b00000000},
	{REG_DIOMAPPING2, 0b00010111},
	{REG_LNA, RF_LNA_ZIN_50 | RF_LNA_GAINSELECT_AUTO},
	{REG_ENDOFCONFIG, 0}
};

const uint8_t rfm69_config_receive[][2] = {
	{REG_PACKETCONFIG1, RF_PACKET1_FORMAT_FIXED | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF},
	{REG_PAYLOADLENGTH, 0},
	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111}, // disable amplifier (otherwise we can't receive)
	{REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95}, // OCP on
	{REG_TESTPA1, 0x55}, // Disable high power mode (only in hw version)
	{REG_TESTPA2, 0x70}, // Disable high power mode (only in hw version)
	{REG_ENDOFCONFIG, 0}
};

const uint8_t rfm69_config_transmit[][2] = {
	{REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF},
	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON}, // w/ power amplifier: PA 1 and 2 on, 20dBm
	{REG_OCP, RF_OCP_OFF | RF_OCP_TRIM_120}, // OCP off, trimming at 120mA
	{REG_TESTPA1, 0x5D}, // High power mode (only in hw version)
	{REG_TESTPA2, 0x7C}, // High power mode (only in hw version)
	{REG_ENDOFCONFIG, 0}
};

const uint8_t rfm69_type_registers_transmit[NUM_TYPE_CONFIGURATIONS] = {
	REG_BITRATEMSB,
	REG_BITRATELSB,
	REG_PAYLOADLENGTH,
};

const uint8_t rfm69_type_configurations_transmit[NUM_TYPES][NUM_TYPE_CONFIGURATIONS] = {
	{TYPE_A_C_BITRATEMSB, TYPE_A_C_BITRATELSB, TYPE_A_C_PACKET_LENGTH},
	{TYPE_B_BITRATEMSB, TYPE_B_BITRATELSB, TYPE_B_PACKET_LENGTH},
	{TYPE_B_BITRATEMSB, TYPE_B_BITRATELSB, TYPE_B_DIM_PACKET_LENGTH},
};


void __attribute__((optimize("-O3"))) rfm69_rx_irq_handler(void) {
	while(!XMC_USIC_CH_RXFIFO_IsEmpty(RFM69_USIC)) {
		rfm69.data_spi[rfm69.data_read_index] = RFM69_USIC->OUTR;
		rfm69.data_read_index++;
	}
}

void __attribute__((optimize("-O3"))) rfm69_tx_irq_handler(void) {
	while(!XMC_USIC_CH_TXFIFO_IsFull(RFM69_USIC)) {
		RFM69_USIC->IN[0] = rfm69.data_spi[rfm69.data_write_index];
		rfm69.data_write_index++;
		if(rfm69.data_write_index >= rfm69.data_length) {
			XMC_USIC_CH_TXFIFO_DisableEvent(RFM69_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
			return;
		}
	}
}

void rfm69_task_spi_transceive(void) {
	XMC_SPI_CH_EnableSlaveSelect(RFM69_USIC, XMC_SPI_CH_SLAVE_SELECT_0);
	XMC_USIC_CH_TXFIFO_EnableEvent(RFM69_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
	XMC_USIC_CH_TriggerServiceRequest(RFM69_USIC, RFM69_SERVICE_REQUEST_TX);

	while(rfm69.data_read_index < rfm69.data_length) {
		rfm69_rx_irq_handler();
		coop_task_yield();
	}

	XMC_SPI_CH_DisableSlaveSelect(RFM69_USIC);
}

void rfm69_task_read_register(const uint8_t reg, uint8_t *data, const uint8_t length) {
	rfm69.data_read_index  = 0;
	rfm69.data_write_index = 0;
	rfm69.data_spi[0] = reg;
	for(uint16_t i = 0; i < length; i++) {
		rfm69.data_spi[i+1] = reg + 1 + i;
	}

	rfm69.data_length = length + 1;

	rfm69_task_spi_transceive();

	memcpy(data, &rfm69.data_spi[1], length);
}

void rfm69_task_write_register(const uint8_t reg, const uint8_t *data, const uint8_t length) {
	rfm69.data_read_index  = 0;
	rfm69.data_write_index = 0;
	rfm69.data_spi[0] = reg | REG_WRITE;
	memcpy(&rfm69.data_spi[1], data, length);

	rfm69.data_length = length + 1;

	rfm69_task_spi_transceive();
}

void rfm69_task_configure_receive(void) {
	// Set general receive config
	for(uint8_t i = 0; rfm69_config_receive[i][0] != REG_ENDOFCONFIG; i++) {
		rfm69_task_write_register(rfm69_config_receive[i][0], &rfm69_config_receive[i][1], 1);
	}

	// Set bitrate in correspondence with remote type
	uint8_t bitrate_data;
	switch(rfm69.remote_type) {
		case REMOTE_SWITCH_V2_REMOTE_TYPE_A: bitrate_data = TYPE_A_RECV_BITRATEMSB; break;
		case REMOTE_SWITCH_V2_REMOTE_TYPE_B: bitrate_data = TYPE_B_RECV_BITRATEMSB; break;
		case REMOTE_SWITCH_V2_REMOTE_TYPE_C: bitrate_data = TYPE_C_RECV_BITRATEMSB; break;
		default: bitrate_data = TYPE_A_RECV_BITRATEMSB; break;
	}
	rfm69_task_write_register(REG_BITRATEMSB, &bitrate_data, 1);

	switch(rfm69.remote_type) {
		case REMOTE_SWITCH_V2_REMOTE_TYPE_A: bitrate_data = TYPE_A_RECV_BITRATELSB; break;
		case REMOTE_SWITCH_V2_REMOTE_TYPE_B: bitrate_data = TYPE_B_RECV_BITRATELSB; break;
		case REMOTE_SWITCH_V2_REMOTE_TYPE_C: bitrate_data = TYPE_C_RECV_BITRATELSB; break;
		default: bitrate_data = TYPE_A_RECV_BITRATELSB; break;
	}
	rfm69_task_write_register(REG_BITRATELSB, &bitrate_data, 1);


	// Change to standby mode
	uint8_t data_opmode;
	rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	while(!(data_opmode & RF_OPMODE_STANDBY)) {
		uint8_t data_irqflags2;
		rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		if((data_opmode != RF_OPMODE_TRANSMITTER) || (!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
			uint8_t data = RF_OPMODE_STANDBY;
			rfm69_task_write_register(REG_OPMODE, &data, 1);
		}

		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	}

	// Enable receiver
	uint8_t data = RF_OPMODE_RECEIVER;
	rfm69_task_write_register(REG_OPMODE, &data, 1);

	// Wait for receiver to be ready
	uint8_t data_irqflags1 = 0;
	do {
		rfm69_task_read_register(REG_IRQFLAGS1, &data_irqflags1, 1);
	} while(!(data_irqflags1 & RF_IRQFLAGS1_RXREADY));


	// Reset command data
	rfm69.data_receive_command_bit = 0;
	rfm69.data_receive_command_new = 0;
}

void rfm69_task_configure_transmit(void) {
	// Set general transmit config
	for(uint8_t i = 0; rfm69_config_transmit[i][0] != REG_ENDOFCONFIG; i++) {
		rfm69_task_write_register(rfm69_config_transmit[i][0], &rfm69_config_transmit[i][1], 1);
	}

	// Set type specific transmit config
	for(uint8_t i = 0; i < NUM_TYPE_CONFIGURATIONS; i++) {
		rfm69_task_write_register(rfm69_type_registers_transmit[i], &rfm69_type_configurations_transmit[rfm69.switch_type_new][i], 1);
	}

	// Make sure that we are in standby mode
	uint8_t data_opmode;
	rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	while(!(data_opmode & RF_OPMODE_STANDBY)) {
		uint8_t data_irqflags2;
		rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		if((data_opmode != RF_OPMODE_TRANSMITTER) || (!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
			uint8_t data = RF_OPMODE_STANDBY;
			rfm69_task_write_register(REG_OPMODE, &data, 1);
		}

		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	}

	// Update switch type
	rfm69.data_switch_length = rfm69_type_configurations_transmit[rfm69.switch_type_new][2];
	rfm69.switch_type_current = rfm69.switch_type_new;
}

void rfm69_task_switch(void) {
	// Change to standby mode
	uint8_t data_opmode;
	rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	while(!(data_opmode & RF_OPMODE_STANDBY)) {
		uint8_t data_irqflags2;
		rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		if((data_opmode != RF_OPMODE_TRANSMITTER) || (!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
			uint8_t data = RF_OPMODE_STANDBY;
			rfm69_task_write_register(REG_OPMODE, &data, 1);
		}

		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	}

	// Configure for transmit
	rfm69_task_configure_transmit();

	for(uint16_t i = 0; i < rfm69.switch_repeats; i++) {
		// Put data in fifo
		rfm69_task_write_register(REG_FIFO, rfm69.data_switch, rfm69.data_switch_length);

		// Activate transmitter
		uint8_t data = RF_OPMODE_TRANSMITTER;
		rfm69_task_write_register(REG_OPMODE, &data, 1);

		// Wait until transmit begins
		uint8_t data_irqflags1 = 0;
		do {
			rfm69_task_read_register(REG_IRQFLAGS1, &data_irqflags1, 1);
		} while(!(data_irqflags1 & RF_IRQFLAGS1_TXREADY));

		// Wait until fifo empty
		uint8_t data_irqflags2 = 0;
		do {
			rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		} while(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY);

		// Wait until we are in standby again
		do {
			rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
		} while(!(data_opmode & RF_OPMODE_STANDBY));
	}

	// Go back to receive mode
	rfm69_task_configure_receive();

	// Wait for 200ms to be sure that send is done,
	// also we can't to transmit data in rapid succession
	coop_task_sleep_ms(300);

	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_READY;
	rfm69.switching_done_send_callback = true;

	// Clear fifo
	while(XMC_GPIO_GetInput(RFM69_FIFO_NOT_EMPTY_PIN)) {
		uint8_t data;
		rfm69_task_read_register(REG_FIFO, &data, 1);
	}
}

// Walk bits from MSB to LSB
inline void rfm69_task_increase_receive_bit(void) {
	if(rfm69.data_receive_start_bit == 0) {
		rfm69.data_receive_start_bit = 31;
		rfm69.data_receive_start = (rfm69.data_receive_start + 1) & DATA_RECEIVE_BUFFER_MASK;
	} else {
		rfm69.data_receive_start_bit--;
	}
}

inline uint8_t rfm69_task_current_receive_bit(void) {
	return (rfm69.data_receive[rfm69.data_receive_start] & (1 << rfm69.data_receive_start_bit)) ? 1 : 0;
}

int32_t rfm69_task_get_digitized_fill_level(void) {
	int32_t bytes = rfm69.data_receive_digitized_end - rfm69.data_receive_digitized_start;
	if(bytes < 0) {
		bytes += DATA_RECEIVE_DIGITIZED_BUFFER_MASK;
	}

	return bytes;
}

void rfm69_task_handle_buffer(void) {
	static uint32_t bit_count = 0;
	static uint8_t data = 0;
	static uint8_t data_shift = 0;
	static uint8_t current_bit = 0;

	while((rfm69.data_receive_start != rfm69.data_receive_end) && (rfm69_task_get_digitized_fill_level() < (DATA_RECEIVE_DIGITIZED_BUFFER_SIZE-4))) {
		uint8_t next_bit = rfm69_task_current_receive_bit();
		rfm69_task_increase_receive_bit();

		if(next_bit == current_bit) {
			bit_count++;
		} else {
			uint8_t num = 0;
			// Interpret 2x oversampled input as "analog values" and allow some
			// inaccuracies in the protocol. In tests we found that the bit rate from
			// remote to remote can differ by ~25%.
			// Example: If we expect a "1" in the on-off-keying we expect to see "11"
			// in the data stream (2x oversampling). But since we know that in the protocol
			// the number of successive ones can only be 1 or 3, we can interpret
			// "1", "11" and "111" as a digitized one etc.
			if((rfm69.remote_type == REMOTE_SWITCH_V2_REMOTE_TYPE_A) || (rfm69.remote_type == REMOTE_SWITCH_V2_REMOTE_TYPE_C)) {
				if(bit_count <= 3) {
					num = 1; // Up to 3 bits in a row is interpreted as 1
				} else if(bit_count <= 9) {
					num = 3; // Up to 9 bits in a row is interpreted as 3
				} else if(bit_count > 20) {
					num = 8+7; // More than 20 is interpreted as 15 (sync)
				} else {
					num = 2; // We explicitly disallow everything in-between, we set 2 bits for it, since that is not allowed in the protocol
				}
			} else { // Type B
				if(bit_count <= 5) {
					num = 1; // Up to 5 bits in a row is interpreted as 1
				} else if(bit_count <= 14) {
					if(current_bit) {
						num = 2; // We explicitly disallow between 6 and 13 ones. We set 2 bits for it, since that is not allowed in the protocol
					} else {
						num = 5; // Up to 14 bits in a row is interpreted as 1
					}
				} else {
					num = 8+7; // More than 14 is interpreted as 15 (sync)
				}
			}

			// Put digitized data into fifo
			for(uint8_t i = 0; i < num; i++) {
				data |= (current_bit << (7-data_shift));
				data_shift++;
				if(data_shift == 8) {
					rfm69.data_receive_digitized[rfm69.data_receive_digitized_end] = data;

					data_shift = 0;
					data = 0;

					rfm69.data_receive_digitized_end = (rfm69.data_receive_digitized_end + 1) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK;
				}
			}

			bit_count = 1;
			current_bit = next_bit;
		}
	}


	int32_t bytes = rfm69_task_get_digitized_fill_level();

	// Only try to handle the data if there is enough data in the buffer for at least one command
	while(bytes > (rfm69.data_receive_command_length+1)) {
		// Now we try to interpret the data that was "digitized" above.
		for(uint8_t i = 0; i < rfm69.data_receive_command_length; i++) {
			uint8_t data_l = rfm69.data_receive_digitized[(rfm69.data_receive_digitized_start + i) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK];
			uint8_t data_h = rfm69.data_receive_digitized[(rfm69.data_receive_digitized_start + i + 1) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK];

			uint8_t data8 = ((data_l << rfm69.data_receive_digitized_start_bit) & 0xFF) | ((data_h >> (8 - rfm69.data_receive_digitized_start_bit)) & 0xFF);

			// type a
			if((rfm69.remote_type == REMOTE_SWITCH_V2_REMOTE_TYPE_A) || (rfm69.remote_type == REMOTE_SWITCH_V2_REMOTE_TYPE_C)) {
				if(((data8 == RFM69_DATA_ON) || (data8 == RFM69_DATA_FLOAT)) ||
				   ((rfm69.data_receive_command_bit == 12) && (data8 == RFM69_DATA_SYNC1)) ||
				   ((rfm69.data_receive_command_bit == 13) && (data8 == RFM69_DATA_SYNC2))) {
					if(data8 == RFM69_DATA_ON) {
						rfm69.data_receive_command_new |= (1 << rfm69.data_receive_command_bit);
					}

					rfm69.data_receive_command_bit++;

					if(rfm69.data_receive_command_bit == rfm69.data_receive_command_length) {
						if(rfm69.data_receive_command_last[rfm69.remote_type] == rfm69.data_receive_command_new) {
							rfm69.data_receive_command_count[rfm69.remote_type]++;
						} else {
							rfm69.data_receive_command_count[rfm69.remote_type] = 1;
						}
						rfm69.data_receive_command_last[rfm69.remote_type] = rfm69.data_receive_command_new;

						rfm69.data_receive_command_bit = 0;
						rfm69.data_receive_command_new = 0;
						rfm69.data_receive_digitized_start = (rfm69.data_receive_digitized_start + rfm69.data_receive_command_length) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK;
					}
				} else {
					rfm69.data_receive_command_bit = 0;
					rfm69.data_receive_command_new = 0;
					rfm69.data_receive_digitized_start_bit++;
					if(rfm69.data_receive_digitized_start_bit == 8) {
						rfm69.data_receive_digitized_start_bit = 0;
						rfm69.data_receive_digitized_start = (rfm69.data_receive_digitized_start + 1) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK;
					}

					break;
				}
			} else {
				// type b
				// TODO: Add support for dimming (in case of dimming, the end sync moves by 4 bits
				if(((data8 == RFM69_DATA_B_0) || (data8 == RFM69_DATA_B_1)) ||
				   ((rfm69.data_receive_command_bit == 0) /*&& (data8 == 0b00000100)*/) || // Do not require leading "0b00000100", this gives lots of false positives because of the amount of 0s
				   ((rfm69.data_receive_command_bit == 1) && (data8 == RFM69_DATA_SYNC2)) ||
				   ((rfm69.data_receive_command_bit == 34) && (data8 == RFM69_DATA_SYNC1)) ||
				   ((rfm69.data_receive_command_bit == 35) && (data8 == RFM69_DATA_SYNC2))) {
					if(data8 == RFM69_DATA_B_1) {
						rfm69.data_receive_command_new |= (((uint64_t)1) << rfm69.data_receive_command_bit);
					}

					rfm69.data_receive_command_bit++;

					if(rfm69.data_receive_command_bit == rfm69.data_receive_command_length) {
						if(rfm69.data_receive_command_last[rfm69.remote_type] == rfm69.data_receive_command_new) {
							rfm69.data_receive_command_count[rfm69.remote_type]++;
						} else {
							rfm69.data_receive_command_count[rfm69.remote_type] = 1;
						}
						rfm69.data_receive_command_last[rfm69.remote_type] = rfm69.data_receive_command_new;

						rfm69.data_receive_command_bit = 0;
						rfm69.data_receive_command_new = 0;
						rfm69.data_receive_digitized_start = (rfm69.data_receive_digitized_start + rfm69.data_receive_command_length) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK;
					}
				} else {
					rfm69.data_receive_command_bit = 0;
					rfm69.data_receive_command_new = 0;
					rfm69.data_receive_digitized_start_bit++;
					if(rfm69.data_receive_digitized_start_bit == 8) {
						rfm69.data_receive_digitized_start_bit = 0;
						rfm69.data_receive_digitized_start = (rfm69.data_receive_digitized_start + 1) & DATA_RECEIVE_DIGITIZED_BUFFER_MASK;
					}

					break;
				}
			}
		}

		bytes = rfm69_task_get_digitized_fill_level();
	}
}

void rfm69_task_tick(void) {
	// Give the RFM69 some time to wake up properly
	coop_task_sleep_ms(50);

	// Set general config that is used for send and receive
	for(uint8_t i = 0; rfm69_config_general[i][0] != REG_ENDOFCONFIG; i++) {
		rfm69_task_write_register(rfm69_config_general[i][0], &rfm69_config_general[i][1], 1);
	}

	// Change to receive mode
	rfm69_task_configure_receive();

	uint8_t shift = 24;
	uint32_t data32 = 0;
	while(true) {
		// If the user wants to turn a remote switch on/off, we do this first
		if(rfm69.switching_state == REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY) {
			rfm69_task_switch();
		// Else, if there is data in the FIFO we handle it
		} else if(XMC_GPIO_GetInput(RFM69_FIFO_NOT_EMPTY_PIN)) {
			if(rfm69.remote_update) {
				rfm69_task_configure_receive();
				rfm69.remote_update = false;
			}

			uint8_t data;
			rfm69_task_read_register(REG_FIFO, &data, 1);
			switch(shift) {
				case 0: {
					rfm69.data_receive[rfm69.data_receive_end] = data32 | data;
					rfm69.data_receive_end = (rfm69.data_receive_end + 1) & DATA_RECEIVE_BUFFER_MASK;

					shift = 24;
					data32 = 0;

					rfm69_task_handle_buffer();
					break;
				}

				default: {
					data32 |= (data << shift);
					shift -= 8;
					break;
				}
			}
		} else {
			coop_task_yield();
		}
	}
}

void rfm69_init_spi(void) {
	// USIC channel configuration
	const XMC_SPI_CH_CONFIG_t channel_config = {
		.baudrate       = RFM69_SPI_BAUDRATE,
		.bus_mode       = XMC_SPI_CH_BUS_MODE_MASTER,
		.selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
		.parity_mode    = XMC_USIC_CH_PARITY_MODE_NONE
	};

	// MOSI pin configuration
	const XMC_GPIO_CONFIG_t mosi_pin_config = {
		.mode             = RFM69_MOSI_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// MISO pin configuration
	const XMC_GPIO_CONFIG_t miso_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};

	// SCLK pin configuration
	const XMC_GPIO_CONFIG_t sclk_pin_config = {
		.mode             = RFM69_SCLK_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// SELECT pin configuration
	const XMC_GPIO_CONFIG_t select_pin_config = {
		.mode             = RFM69_SELECT_PIN_AF,
		.output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
	};

	// Configure MISO pin
	XMC_GPIO_Init(RFM69_MISO_PIN, &miso_pin_config);

	// Initialize USIC channel in SPI master mode
	XMC_SPI_CH_Init(RFM69_USIC, &channel_config);
	RFM69_USIC->SCTR &= ~USIC_CH_SCTR_PDL_Msk; // Set passive data level to 0
	RFM69_USIC->PCR_SSCMode &= ~USIC_CH_PCR_SSCMode_TIWEN_Msk; // Disable time between bytes

	XMC_SPI_CH_SetBitOrderMsbFirst(RFM69_USIC);

	XMC_SPI_CH_SetWordLength(RFM69_USIC, (uint8_t)8U);
	XMC_SPI_CH_SetFrameLength(RFM69_USIC, (uint8_t)64U);

	XMC_SPI_CH_SetTransmitMode(RFM69_USIC, XMC_SPI_CH_MODE_STANDARD);

	// Configure the clock polarity and clock delay
	XMC_SPI_CH_ConfigureShiftClockOutput(RFM69_USIC,
									     XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_ENABLED,
									     XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
	// Configure Leading/Trailing delay
	XMC_SPI_CH_SetSlaveSelectDelay(RFM69_USIC, 2);


	// Set input source path
	XMC_SPI_CH_SetInputSource(RFM69_USIC, RFM69_MISO_INPUT, RFM69_MISO_SOURCE);

	// SPI Mode: CPOL=1 and CPHA=1
	RFM69_USIC_CHANNEL->DX1CR |= USIC_CH_DX1CR_DPOL_Msk;

	// Configure transmit FIFO
	XMC_USIC_CH_TXFIFO_Configure(RFM69_USIC, 48, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);

	// Configure receive FIFO
	XMC_USIC_CH_RXFIFO_Configure(RFM69_USIC, 32, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);

	// Set service request for tx FIFO transmit interrupt
	XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(RFM69_USIC, XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD, RFM69_SERVICE_REQUEST_TX);  // IRQ RFM69_IRQ_TX

	// Set service request for rx FIFO receive interrupt
	XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(RFM69_USIC, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD, RFM69_SERVICE_REQUEST_RX);  // IRQ RFM69_IRQ_RX
	XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(RFM69_USIC, XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE, RFM69_SERVICE_REQUEST_RX); // IRQ RFM69_IRQ_RX

	//Set priority and enable NVIC node for transmit interrupt
	NVIC_SetPriority((IRQn_Type)RFM69_IRQ_TX, RFM69_IRQ_TX_PRIORITY);
	NVIC_EnableIRQ((IRQn_Type)RFM69_IRQ_TX);

	// Set priority and enable NVIC node for receive interrupt
	NVIC_SetPriority((IRQn_Type)RFM69_IRQ_RX, RFM69_IRQ_RX_PRIORITY);
	NVIC_EnableIRQ((IRQn_Type)RFM69_IRQ_RX);

	// Start SPI
	XMC_SPI_CH_Start(RFM69_USIC);

	// Configure SCLK pin
	XMC_GPIO_Init(RFM69_SCLK_PIN, &sclk_pin_config);

	// Configure slave select pin
	XMC_GPIO_Init(RFM69_SELECT_PIN, &select_pin_config);

	// Configure MOSI pin
	XMC_GPIO_Init(RFM69_MOSI_PIN, &mosi_pin_config);

	XMC_USIC_CH_RXFIFO_Flush(RFM69_USIC);
	XMC_USIC_CH_RXFIFO_EnableEvent(RFM69_USIC, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
}

void rfm69_init(void) {
	memset(&rfm69, 0, sizeof(RFM69));
	rfm69.switch_repeats = 5;
	rfm69.switch_type_current = 255;
	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_READY;

	rfm69.data_receive_command_length = TYPE_A_C_PACKET_LENGTH_RECEIVE;

	rfm69.remote_callback_enabled = false;
	rfm69.remote_minimum_repeats = 2;
	rfm69.remote_type = REMOTE_SWITCH_V2_REMOTE_TYPE_A;

	rfm69.data_receive_start_bit = 31;

	// FIFO pin configuration
	const XMC_GPIO_CONFIG_t fifo_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};
	XMC_GPIO_Init(RFM69_FIFO_NOT_EMPTY_PIN, &fifo_pin_config);
	XMC_GPIO_SetHardwareControl(RFM69_FIFO_NOT_EMPTY_PIN, XMC_GPIO_HWCTRL_DISABLED);

	// Reset pin configuration (default = floating)
	const XMC_GPIO_CONFIG_t reset_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};
	XMC_GPIO_Init(RFM69_RESET_PIN, &reset_pin_config);
	XMC_GPIO_SetHardwareControl(RFM69_RESET_PIN, XMC_GPIO_HWCTRL_DISABLED);

	rfm69_init_spi();
	coop_task_init(&rfm69_task, rfm69_task_tick);
}

void rfm69_tick(void) {
	coop_task_tick(&rfm69_task);
}
