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

#ifndef MCU_SENSOR_H
#define MCU_SENSOR_H

/********************************************
 * INCLUDES                                 *
 ********************************************/

#include <stdint.h>

/********************************************
 * EXPORTED TYPES DEFINITIONS               *
 ********************************************/

typedef union
{
  uint32_t als;
  uint8_t  pir;
  uint32_t power;
  uint16_t current; 
  uint16_t voltage;
  uint32_t energy;
} SensorValue_T;

typedef enum
{
  PRESENCE_DETECTED           = 0x004D,
  PRESENT_AMBIENT_LIGHT_LEVEL = 0x004E,
  PRESENT_DEVICE_INPUT_POWER  = 0x0052,
  PRESENT_INPUT_CURRENT       = 0x0057,
  PRESENT_INPUT_VOLTAGE       = 0x0059,
  TOTAL_DEVICE_ENERGY_USE     = 0x006A
} SensorProperty_T;

/********************************************
 * EXPORTED #define CONSTANTS AND MACROS    *
 ********************************************/

#define MESH_PROPERTY_PRESENT_AMBIENT_LIGHT_LEVEL_UNKNOWN_VAL    0xFFFFFF
#define MESH_PROPERTY_PRESENT_DEVICE_INPUT_POWER_UNKNOWN_VAL     0xFFFFFF
#define MESH_PROPERTY_PRESENT_INPUT_CURRENT_UNKNOWN_VAL          0xFFFF
#define MESH_PROPERTY_PRESENT_INPUT_VOLTAGE_UNKNOWN_VAL          0xFFFF
#define MESH_PROPERTY_TOTAL_DEVICE_ENERGY_USE_UNKNOWN_VAL        0xFFFFFF

/********************************************
 * EXPORTED FUNCTIONS PROTOTYPES            *
 ********************************************/

/*
 *  Set Sensor Client instance index
 *
 *  @param idx  Lightness value
 */
void SetInstanceIdxSensor(uint8_t idx);

/*
 *  Get Sensor Client instance index
 *
 *  @return     Lightness value
 */
uint8_t GetInstanceIdxSensor(void);

/*
 *  Process ALS value update
 *
 *  @param src_addr            Source address
 *  @param sensor_value        New sensor value
 */
void ProcessPresentAmbientLightLevel(uint16_t src_addr, SensorValue_T sensor_value);

/*
 *  Process PIR value update
 *
 *  @param src_addr            Source address
 *  @param sensor_value        New sensor value
 */
void ProcessPresenceDetected(uint16_t src_addr, SensorValue_T sensor_value);

/*
 *  Process Power value update
 *
 *  @param sensor_value       New Power value
 *  @param src_addr           Source address
 */
void ProcessPresentDeviceInputPower(uint16_t src_addr, SensorValue_T sensor_value);

/*
 *  Process Current value update
 *
 *  @param sensor_value       New Current value
 *  @param src_addr           Source address
 */
void ProcessPresentInputCurrent(uint16_t src_addr, SensorValue_T sensor_value);

/*
 *  Process Voltage value update
 *
 *  @param sensor_value       New Voltage value
 *  @param src_addr           Source address
 */
void ProcessPresentInputVoltage(uint16_t src_addr, SensorValue_T sensor_value);

/*
 *  Process Energy value update
 *
 *  @param sensor_value       New Energy value
 *  @param src_addr           Source address
 */
void ProcessTotalDeviceEnergyUse(uint16_t src_addr, SensorValue_T sensor_value);

/*
 *  Setup sensor server hardware
 */
void SetupSensor(void);

#endif  // MCU_SENSOR_H
