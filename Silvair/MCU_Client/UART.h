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

#ifndef UART_H_
#define UART_H_

/********************************************
 * INCLUDES                                 *
 ********************************************/

#include "stdint.h"
#include "Config.h"

/********************************************
 * EXPORTED #define CONSTANTS AND MACROS    *
 ********************************************/

/**< Defines maximum data length in frame */
#define MAX_PAYLOAD_SIZE 127

#if LOG_INFO_ENABLE == 1
  #define INFO(f_, ...) DEBUG_INTERFACE.printf((f_), ##__VA_ARGS__)
#else
  #define INFO(f_, ...)
#endif

#if LOG_DEBUG_ENABLE == 1
  #define DEBUG(f_, ...) DEBUG_INTERFACE.printf((f_), ##__VA_ARGS__)
#else
  #define DEBUG(f_, ...)
#endif

/********************************************
 * EXPORTED FUNCTIONS PROTOTYPES            *
 ********************************************/

/*
 *  Setup UART hardware
 */
void UART_Init(void);

/*
 *  Send Ping Request command
 */
void UART_SendPingRequest(void);

/*
 *  Send Pong Response command
 */
void UART_SendPongResponse(void);

/*
 *  Send Software Reset Request command
 */
void UART_SendSoftwareResetRequest(void);

/*
 *  Send Create Instances Request command
 *
 *  @param * model_id   Pointer to model ids list
 *  @param len          Model ids list length
 */
void UART_SendCreateInstancesRequest(uint8_t * model_id, uint8_t len);

/*
 *  Send Mesh Message Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendMeshMessageRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Send Sensor Update Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendSensorUpdateRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Send Firmware Version Set Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendFirmwareVersionSetRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Send Start Node Request command
 */
void UART_StartNodeRequest(void);

/*
 *  Send Firmware Version Request command
 */
void UART_ModemFirmwareVersionRequest(void);

/*
 *  Send Dfu Init Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendDfuInitResponse(uint8_t * p_payload, uint8_t len);

/*
 *  Send Dfu Status Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendDfuStatusResponse(uint8_t * p_payload, uint8_t len);

/*
 *  Send Dfu Page Create command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendDfuPageCreateResponse(uint8_t * p_payload, uint8_t len);

/*
 *  Send Dfu Page Store Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendDfuPageStoreResponse(uint8_t * p_payload, uint8_t len);

/*
 *  Send Dfu State Check Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendDfuStateCheckRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Send Dfu Cancel Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void UART_SendDfuCancelRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Flush UART
 */
void UART_Flush();

/*
 *  Receive and process incoming UART command
 */
void UART_ProcessIncomingCommand(void);

/*
 *  Process Init Device Event command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessEnterInitDevice(uint8_t * p_payload, uint8_t len);

/*
 *  Process Create Instances Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessEnterDevice(uint8_t * p_payload, uint8_t len);

/*
 *  Process Init Node Event command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessEnterInitNode(uint8_t * p_payload, uint8_t len);

/*
 *  Process Start Node Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessEnterNode(uint8_t * p_payload, uint8_t len);

/*
 *  Process Mesh Message Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessMeshCommand(uint8_t * p_payload, uint8_t len);

/*
 *  Process Attention Event command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessAttention(uint8_t * p_payload, uint8_t len);

/*
 *  Process Error command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessError(uint8_t * p_payload, uint8_t len);

/*
 *  Process Modem Firmware Version Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessModemFirmwareVersion(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu Init Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuInitRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu Status Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuStatusRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu Page Create Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuPageCreateRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu Write Data Event command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuWriteDataEvent(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu Pahe Store Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuPageStoreRequest(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu State Check Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuStateCheckResponse(uint8_t * p_payload, uint8_t len);

/*
 *  Process Dfu Cancel Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
extern void ProcessDfuCancelResponse(uint8_t * p_payload, uint8_t len);

/*
 *  Process FactoryResetEvent
 */
extern void ProcessFactoryResetEvent(void);

#endif  // UART_H_
