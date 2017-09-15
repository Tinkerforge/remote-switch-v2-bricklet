/* remote-switch-v2-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_rfm69.h: Configurations for RFM69 ISM transceiver
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

#ifndef CONFIG_RFM69_H
#define CONFIG_RFM69_H

#include "xmc_gpio.h"

#define RFM69_SPI_BAUDRATE         400000
#define RFM69_USIC_CHANNEL         USIC0_CH0
#define RFM69_USIC                 XMC_SPI0_CH0

#define RFM69_SCLK_PIN             P0_14
#define RFM69_SCLK_PIN_AF          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_14_AF_U0C0_SCLKOUT)

#define RFM69_SELECT_PIN           P0_13
#define RFM69_SELECT_PIN_AF        (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6 | P0_13_AF_U0C0_SELO4)

#define RFM69_MOSI_PIN             P2_0
#define RFM69_MOSI_PIN_AF          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6 | P2_0_AF_U0C0_DOUT0)

#define RFM69_MISO_PIN             P0_15
#define RFM69_MISO_INPUT           XMC_USIC_CH_INPUT_DX0
#define RFM69_MISO_SOURCE          0b001 // DX0B

#define RFM69_SERVICE_REQUEST_RX   2
#define RFM69_SERVICE_REQUEST_TX   3

#define RFM69_RESET_PIN            P2_1
#define RFM69_FIFO_NOT_EMPTY_PIN   P1_0

#define RFM69_IRQ_RX               11
#define RFM69_IRQ_RX_PRIORITY      2
#define RFM69_IRQ_TX               12
#define RFM69_IRQ_TX_PRIORITY      3


#if 0
#define RFM69_USIC_CHANNEL         USIC0_CH1
#define RFM69_USIC                 XMC_SPI0_CH1

#define RFM69_SCLK_PIN             P0_8
#define RFM69_SCLK_PIN_AF          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_8_AF_U0C1_SCLKOUT)

#define RFM69_SELECT_PIN           P0_9
#define RFM69_SELECT_PIN_AF        (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_9_AF_U0C1_SELO0)

#define RFM69_MOSI_PIN             P0_7
#define RFM69_MOSI_PIN_AF          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_7_AF_U0C1_DOUT0)

#define RFM69_MISO_PIN             P0_6
#define RFM69_MISO_INPUT           XMC_USIC_CH_INPUT_DX0
#define RFM69_MISO_SOURCE          0b010 // DX0C

#endif




#endif
