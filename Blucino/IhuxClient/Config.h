/*
Copyright © 2017 Silvair Sp. z o.o. All Rights Reserved.
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CONFIG_H_
#define CONFIG_H_

/********************************************
 * INCLUDES                                 *
 ********************************************/
 
#include "Arduino.h"

/********************************************
 * LOCAL #define CONSTANTS AND MACROS       *
 ********************************************/

#define BUILD_NUMBER                "0000"                   /**< Defines firmware build number. */
#define DFU_VALIDATION_STRING       "client"                 /**< Defines string to be expected in app data */

#define INSTANCE_INDEX_UNKNOWN      UINT8_MAX /**< Defines unknown instance index value. */

#define ANALOG_MIN                  0         /**< Defines lower range of analog measurements. */
#define ANALOG_MAX                  1023      /**< Defines uppper range of analog measurements. */
#define ANALOG_REFERENCE_VOLTAGE_MV 3300      /**< Defines ADC reference voltage in millivolts */

#define DEBUG_INTERFACE             (Serial)  /**< Defines serial port to print debug messages. */
#define DEBUG_INTERFACE_BAUDRATE    9600      /**< iHux was 115200; defines baudrate of debug interface. */
#define UART_INTERFACE              (Serial2) /**< Defines serial port to communicate with modem. */
#define UART_INTERFACE_BAUDRATE     57600     /**< Defines baudrate of modem interface. */

#define PIN_LED_1                   11  /**< Defines led 1 pin. */
#define PIN_LED_2                   12  /**< Defines led 2 pin. */
#define PIN_LED_STATUS              13  /**< Defines led pin. */
#define PIN_PWM                     16  /**< Defines pwm pin. */
#define PIN_SW_1                    22  /**< Defines switch 1 pin. */
#define PIN_SW_2                    21  /**< Defines switch 2 pin. */
#define PIN_SW_3                    20  /**< Defines switch 3 pin. */
#define PIN_SW_4                    15  /**< Defines switch 4 pin. */
#define PIN_ENCODER_SW              14  /**< Defines encoder switch pin. */
#define PIN_ENCODER_A               6   /**< Defines encoder A pin. */
#define PIN_ENCODER_B               4   /**< Defines encoder B pin. */
#define PIN_ANALOG                  9   /**< Defines analog measurement pin. */

#define BUTTON_DEBOUNCE_TIME_MS     20      /**< Defines buttons debounce time in milliseconds. */
#define ENCODER_DEBOUNCE_TIME_US    300     /**< Defines encoder debounce time in microseconds. */
#define ATTENTION_TIME_MS           500     /**< Defines attention state change time in milliseconds. */
#define DATA_VALIDITY_PERIOD_MS     3000    /**< Defines sensor data validity period. */

#define LOG_INFO_ENABLE             1   /**< Enables INFO level logs */
#define LOG_DEBUG_ENABLE            1   /**< Enables DEBUG level logs Enabling this make DFU impossible, due to implementation of UART in Arduino. */

#define PING_TRACE                  0   // iHux: trace ping protocoll (1) or suppress tracing(0)
#define COMPACT_TRACE               1   // iHux: compact tracing format

#endif // CONFIG_H_
