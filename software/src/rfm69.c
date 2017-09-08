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

const uint8_t rfm69_config[][2] = {
	{REG_FRFMSB, RF_FRFMSB_433}, // 433 Mhz
	{REG_FRFMID, RF_FRFMID_433},
	{REG_FRFLSB, RF_FRFLSB_433},
//	{REG_FDEVMSB, 0x10}, // Frequency deviation
//	{REG_FDEVLSB, 0x00},
	{REG_DATAMODUL, RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00}, // Use OOK
	{REG_BITRATEMSB, TYPE_A_C_BITRATEMSB},
	{REG_BITRATELSB, TYPE_A_C_BITRATELSB},
	{REG_PREAMBLEMSB, 0x00}, // no preamble
	{REG_PREAMBLELSB, 0x00},
	{REG_SYNCCONFIG, RF_SYNC_OFF}, // Use empty sync to assure small pause between commands
	{REG_SYNCVALUE1, 0},
	{REG_SYNCVALUE2, 0},
	{REG_SYNCVALUE3, 0},
	{REG_SYNCVALUE4, 0},
	{REG_SYNCVALUE5, 0},
	{REG_SYNCVALUE6, 0},
	{REG_SYNCVALUE7, 0},
	{REG_SYNCVALUE8, 0},
	{REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF},
	{REG_PAYLOADLENGTH, TYPE_A_C_PACKET_LENGTH},
	{REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY}, // Set fifo threshold, send when threshold reached
	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA0_ON}, // w/o power amplifier
//	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON}, // w/ power amplifier: PA 1 and 2 on, 20dBm
	{REG_OCP, RF_OCP_OFF | RF_OCP_TRIM_120}, // OCP off, trimming at 120mA
//	{REG_TESTPA1, 0x5D}, // High power mode (only in hw version)
//	{REG_TESTPA2, 0x7C}, // High power mode (only in hw version)
	{REG_OPMODE, RF_OPMODE_STANDBY}, // Set mode to transmitter
//	{REG_DIOMAPPING1, 0b01010101},
	{REG_ENDOFCONFIG, 0}
};

const uint8_t rfm69_type_registers[NUM_TYPE_CONFIGURATIONS] = {
	REG_BITRATEMSB,
	REG_BITRATELSB,
	REG_PAYLOADLENGTH,
};

const uint8_t rfm69_type_configurations[NUM_TYPES][NUM_TYPE_CONFIGURATIONS] = {
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
	XMC_SPI_CH_EnableSlaveSelect(RFM69_USIC, XMC_SPI_CH_SLAVE_SELECT_4);
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
	uint8_t data_opmode;
	rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	while(!(data_opmode & RF_OPMODE_STANDBY)) {
		uint8_t data_irqflags2;
		rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
			uint8_t data = RF_OPMODE_STANDBY;
			rfm69_task_write_register(REG_OPMODE, &data, 1);
		}

		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	}

	uint8_t data_modul = RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00;
	rfm69_task_write_register(REG_DATAMODUL, &data_modul, 1);

	uint8_t data = RF_OPMODE_RECEIVER;
	rfm69_task_write_register(REG_OPMODE, &data, 1);
}

void rfm69_task_configure_transmit(void) {
	uint8_t data_opmode;
	rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	while(!(data_opmode & RF_OPMODE_STANDBY)) {
		uint8_t data_irqflags2;
		rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
			uint8_t data = RF_OPMODE_STANDBY;
			rfm69_task_write_register(REG_OPMODE, &data, 1);
		}

		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	}

	uint8_t data_modul = RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00;
	rfm69_task_write_register(REG_DATAMODUL, &data_modul, 1);
}

void rfm69_task_switch(void) {
	rfm69_task_configure_transmit();

	if(rfm69.switch_type_current != rfm69.switch_type_new) {
		for(uint8_t i = 0; i < NUM_TYPE_CONFIGURATIONS; i++) {
			rfm69_task_write_register(rfm69_type_registers[i], &rfm69_type_configurations[rfm69.switch_type_new][i], 1);
		}

		rfm69.data_switch_length = rfm69_type_configurations[rfm69.switch_type_new][2];
		rfm69.switch_type_current = rfm69.switch_type_new;
	}

	for(uint16_t i = 0; i < rfm69.switch_repeats; i++) {
		uint8_t data_opmode;
		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
		while(!(data_opmode & RF_OPMODE_STANDBY)) {
			uint8_t data_irqflags2;
			rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
			if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
				uint8_t data = RF_OPMODE_STANDBY;
				rfm69_task_write_register(REG_OPMODE, &data, 1);
			}

			rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
		}

		uint8_t data_irqflags2;
		do {
			rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
		} while(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY);

		rfm69_task_write_register(REG_FIFO, rfm69.data_switch, rfm69.data_switch_length);
		uint8_t data = RF_OPMODE_TRANSMITTER;
		rfm69_task_write_register(REG_OPMODE, &data, 1);
	}

	uint8_t data_opmode;
	do {
		rfm69_task_read_register(REG_OPMODE, &data_opmode, 1);
	} while(!(data_opmode & RF_OPMODE_STANDBY));

	uint8_t data_irqflags2;
	rfm69_task_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
	if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
		uint8_t data = RF_OPMODE_STANDBY;
		rfm69_task_write_register(REG_OPMODE, &data, 1);
	}

	coop_task_sleep_ms(200);
	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_READY;
	rfm69.switching_done_send_callback = true;

	rfm69_task_configure_receive();
}

void rfm69_task_tick(void) {
	for(uint8_t i = 0; rfm69_config[i][0] != REG_ENDOFCONFIG; i++) {
		rfm69_task_write_register(rfm69_config[i][0], &rfm69_config[i][1], 1);
	}

	rfm69_task_configure_receive();

	while(true) {
		if(rfm69.switching_state == REMOTE_SWITCH_V2_SWITCHING_STATE_BUSY) {
			rfm69_task_switch();
		}

		coop_task_yield();
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
	XMC_GPIO_Init(RFM69_MOSI_PIN, &miso_pin_config);

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
									     XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_1_DELAY_DISABLED,
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
//	RFM69_USIC->RBCTR |= USIC_CH_RBCTR_SRBTM_Msk; // RX Interrupt for >= 0

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

//	XMC_USIC_CH_EnableEvent(RFM69_USIC, (uint32_t)((uint32_t)XMC_USIC_CH_EVENT_STANDARD_RECEIVE | (uint32_t)XMC_USIC_CH_EVENT_ALTERNATIVE_RECEIVE));

	XMC_USIC_CH_RXFIFO_Flush(RFM69_USIC);
	XMC_USIC_CH_RXFIFO_EnableEvent(RFM69_USIC, XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);
}

void rfm69_init(void) {
	memset(&rfm69, 0, sizeof(RFM69));
	rfm69.switch_repeats = 5;
	rfm69.switch_type_current = 255;
	rfm69.switching_state = REMOTE_SWITCH_V2_SWITCHING_STATE_READY;

	rfm69_init_spi();
	coop_task_init(&rfm69_task, rfm69_task_tick);
}

uint32_t count = 0;
uint32_t t = 0;

void rfm69_tick(void) {
	if(system_timer_is_time_elapsed_ms(t, 1000)) {
		t = system_timer_get_ms();

		logi("tick: %u\n\r", count);
		count++;
	}

	coop_task_tick(&rfm69_task);
}
