/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#ifndef CUSTBD_H
#define CUSTBD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

#if defined(SENSORTAG_R40)
// LEDs definitions for SENSORTAG_R40
#define LEDS_NUMBER    3

#define LED_START      28
#define LED_1          27
#define LED_2          28
#define LED_3          29
//#define LED_4          27
#define LED_STOP       29

#define LEDS_ACTIVE_STATE 0

#define LEDS_INV_MASK  LEDS_MASK

//#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }
#define LEDS_LIST { LED_1, LED_2, LED_3 }

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3
//#define BSP_LED_3      LED_4

#define BUTTONS_NUMBER 2

#define BUTTON_START   18
#define BUTTON_1       18
#define BUTTON_2       3
//#define BUTTON_3       11
//#define BUTTON_4       11
#define BUTTON_STOP    3
#define BUTTON_PULL    NRF_GPIO_PIN_PULLDOWN

#define BUTTONS_ACTIVE_STATE 1

//#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4 }
#define BUTTONS_LIST { BUTTON_1, BUTTON_2 }

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
//#define BSP_BUTTON_2   BUTTON_3
//#define BSP_BUTTON_3   BUTTON_4

#define RX_PIN_NUMBER  17
#define TX_PIN_NUMBER  16
#define CTS_PIN_NUMBER 15
#define RTS_PIN_NUMBER 14
#define HWFC           false
#define SPIM0_SCK_PIN   26  // SPI clock GPIO pin number.
#define SPIM0_MOSI_PIN  25  // SPI Master Out Slave In GPIO pin number.
#define SPIM0_MISO_PIN  31 // SPI Master In Slave Out GPIO pin number.
#define SPIM0_SS_PIN    30 // SPI Slave Select GPIO pin number.

#elif defined (BASIC_SENSOR)
// LEDs definitions for SENSORTAG_R40
#define LEDS_NUMBER    3
#define LED_START      25
#define LED_1          25
#define LED_2          26
#define LED_3          27
#define LED_STOP       27
#define LEDS_ACTIVE_STATE 0
#define LEDS_INV_MASK  LEDS_MASK
#define LEDS_LIST { LED_1, LED_2, LED_3 }
#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3

#define BUTTONS_NUMBER 2
#define BUTTON_START   13
#define BUTTON_1       13
#define BUTTON_2       12
#define BUTTON_STOP    12
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define BUTTONS_ACTIVE_STATE 0
#define BUTTONS_LIST { BUTTON_1, BUTTON_2 }
#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2

#define RX_PIN_NUMBER  22
#define TX_PIN_NUMBER  23
#define CTS_PIN_NUMBER 24
#define RTS_PIN_NUMBER 29
#define HWFC           false
#define SPIM0_SCK_PIN   21  // SPI clock GPIO pin number.
#define SPIM0_MOSI_PIN  14  // SPI Master Out Slave In GPIO pin number.
#define SPIM0_MISO_PIN  15 // SPI Master In Slave Out GPIO pin number.
#define SPIM0_SS_PIN    18 // SPI Slave Select GPIO pin number.

#endif

/**@ Misc defines for other I/O such as SPIS and SPIM1,2

#define SPIS_MISO_PIN   28  // SPI MISO signal.
#define SPIS_CSN_PIN    12  // SPI CSN signal.
#define SPIS_MOSI_PIN   25  // SPI MOSI signal.
#define SPIS_SCK_PIN    29  // SPI SCK signal.

#define SPIM1_SCK_PIN   29   // SPI clock GPIO pin number.
#define SPIM1_MOSI_PIN  28   // SPI Master Out Slave In GPIO pin number.
#define SPIM1_MISO_PIN  30   // SPI Master In Slave Out GPIO pin number.
#define SPIM1_SS_PIN    31   // SPI Slave Select GPIO pin number.

#define SPIM2_SCK_PIN   28  // SPI clock GPIO pin number.
#define SPIM2_MOSI_PIN  29  // SPI Master Out Slave In GPIO pin number.
#define SPIM2_MISO_PIN  30  // SPI Master In Slave Out GPIO pin number.
#define SPIM2_SS_PIN    31  // SPI Slave Select GPIO pin number.
*/

#if defined (S212)
// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
#else

#define NRF_CLOCK_LFCLKSRC      {.source       = NRF_CLOCK_LF_SRC_XTAL,      \
                                 .rc_ctiv      = 0,                          \
                                 .rc_temp_ctiv = 0,                          \
                                 .accuracy     = NRF_CLOCK_LF_ACCURACY_20_PPM}
#endif

#ifdef __cplusplus
}
#endif

#endif // CUSTBD_H
