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

/*******************************************
 * INCLUDES                                *
 *******************************************/

#include "MCU_Health.h"
#include "Arduino.h"
#include "Config.h"
#include "Mesh.h"
#include "UART.h"
#include <limits.h>
#include <stdint.h>

/********************************************
 * LOCAL #define CONSTANTS AND MACROS       *
 ********************************************/

/**
 * Buttons placement definitions
 */
#define PB_FAULT      PIN_SW_1  /**< Defines Fault (used to Set and Clear faults) button location. */
#define PB_CONNECTION PIN_SW_2  /**< Defines Connection (used to disconnect and connect UART) button location. */

/**
 * Used Health messages lengths
 */
#define HEALTH_MESSAGE_SET_FAULT_LEN   4
#define HEALTH_MESSAGE_CLEAR_FAULT_LEN 4
#define TEST_MSG_LEN                   4

#define EXAMPLE_FAULT_ID               0x01u

/********************************************
 * STATIC VARIABLES                         *
 ********************************************/

static volatile bool Fault              = false;                    /**< Implies if Fault button has been pushed */
static volatile bool Connection         = false;                    /**< Implies if Connection button has been pushed */
static bool          FaultState         = false;                    /**< Holds Fault state. True if fault is set and false if fault is cleared */
static bool          ConnectionState    = true;                     /**< Holds Connection state. True if UART connection is up and false if it is down */
static uint8_t       HealthServerIdx    = INSTANCE_INDEX_UNKNOWN;   /**  Index of registered Health Server model */
static bool          TestStarted        = false;                    /**  True, if test is started. */
static uint32_t      TestStartTimestamp = 0;                        /**  Time left to finish test.*/
static uint8_t       TestStartPayload[TEST_MSG_LEN];                /**  Current test payload.*/

/********************************************
 * LOCAL FUNCTION PROTOTYPES                *
 ********************************************/

/*
 *  Send Health Set Fault Request
 *
 *  @param company_id      Company id
 *  @param fault_id        Fault id
 *  @param instance_idx    Instance index
 */
static void MCU_Health_SendSetFaultRequest(uint16_t company_id, uint8_t fault_id, uint8_t instance_idx);

/*
 *  Send Health Clear Fault Request
 *
 *  @param company_id      Company id
 *  @param fault_id        Fault id
 *  @param instance_idx    Instance index
 */
static void MCU_Health_SendClearFaultRequest(uint16_t company_id, uint8_t fault_id, uint8_t instance_idx);

/*
 * Fault button interrupt handler
 */
static void InterruptFaultPBClick(void);

/*
 * Connection button interrupt handler
 */
static void InterruptConnectionPBClick(void);

/********************************************
 * LOCAL FUNCTIONS DEFINITIONS              *
 ********************************************/

static void InterruptFaultPBClick(void)
{
  delay(BUTTON_DEBOUNCE_TIME_MS);
  if (digitalRead(PB_FAULT)) return;

  Fault = true;
}

static void InterruptConnectionPBClick(void)
{
  delay(BUTTON_DEBOUNCE_TIME_MS);
  if (digitalRead(PB_CONNECTION)) return;

  Connection = true;
}

void MCU_Health_SendSetFaultRequest(uint16_t company_id, uint8_t fault_id, uint8_t instance_idx)
{
  uint8_t buf[HEALTH_MESSAGE_SET_FAULT_LEN];
  size_t  index = 0;

  buf[index++] = lowByte(company_id);
  buf[index++] = highByte(company_id);
  buf[index++] = fault_id;
  buf[index++] = instance_idx;

  UART_SendSetFaultRequest(buf, sizeof(buf));
}

void MCU_Health_SendClearFaultRequest(uint16_t company_id, uint8_t fault_id, uint8_t instance_idx)
{
  uint8_t buf[HEALTH_MESSAGE_CLEAR_FAULT_LEN];
  size_t  index = 0;

  buf[index++] = lowByte(company_id);
  buf[index++] = highByte(company_id);
  buf[index++] = fault_id;
  buf[index++] = instance_idx;

  UART_SendClearFaultRequest(buf, sizeof(buf));
}

/********************************************
 * EXPORTED FUNCTION DEFINITIONS            *
 ********************************************/

void ProcessStartTest(uint8_t * p_payload, uint8_t len)
{
  UART_SendTestStartResponse(NULL, 0);
  digitalWrite(PIN_LED_STATUS, true);
  memcpy(TestStartPayload, p_payload, len);
  TestStarted        = true;
  TestStartTimestamp = millis();
}

bool IsTestInProgress(void)
{
  return TestStarted;
}

void SetupHealth(void)
{
  INFO("                   Health initialization.\n");
  pinMode(PIN_LED_1,     OUTPUT);
  pinMode(PIN_LED_2,     OUTPUT);
  pinMode(PB_FAULT,      INPUT_PULLUP);
  pinMode(PB_CONNECTION, INPUT_PULLUP);

  digitalWrite(PIN_LED_1, FaultState);
  digitalWrite(PIN_LED_2, ConnectionState);
 
  attachInterrupt(digitalPinToInterrupt(PB_FAULT),      InterruptFaultPBClick,      FALLING);
  attachInterrupt(digitalPinToInterrupt(PB_CONNECTION), InterruptConnectionPBClick, FALLING);
}

void LoopHealth(void)
{
  if (Fault)
  {
    Fault = false;
    INFO("Fault button\n");
    FaultState = !FaultState;
    digitalWrite(PIN_LED_1, FaultState);
    
    if(FaultState)
    {
      MCU_Health_SendSetFaultRequest(SILVAIR_ID, EXAMPLE_FAULT_ID, HealthServerIdx);
    }
    else
    {
      MCU_Health_SendClearFaultRequest(SILVAIR_ID, EXAMPLE_FAULT_ID, HealthServerIdx);
    }
  }

  if (Connection)
  {
    Connection = false;
    INFO("Connection button\n");
    ConnectionState = !ConnectionState;
    digitalWrite(PIN_LED_2, ConnectionState);
    
    if(ConnectionState)
    {
      UART_EnablePings();
    }
    else
    {
      UART_DisablePings();
    }
  }

  if (IsTestInProgress())
  {
    uint32_t timestamp = millis();
    uint32_t duration  = timestamp - TestStartTimestamp;
    if (duration >= TEST_TIME_MS)
    {
      TestStarted = false;
      digitalWrite(PIN_LED_STATUS, false);
      UART_SendTestFinishedRequest(TestStartPayload, TEST_MSG_LEN);
    }
  }
}

void SetHealthServerIdx(uint8_t idx)
{
  HealthServerIdx = idx;
}

uint8_t GetHealthServerIdx(void)
{
  return HealthServerIdx;
}
