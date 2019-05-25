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

/********************************************
* INCLUDES                                 *
********************************************/

#include "Config.h"
//#include "MCU_Health.h"
#include "MCU_Lightness.h"
//#include "MCU_Sensor.h"
//#include "MCU_DFU.h"
#include "MCU_Attention.h"
#include "Mesh.h"
#include "UART.h"
//#include "SDM.h"
#include <math.h>
#include <string.h>

/********************************************
 * iHux debugging support                   *
 ********************************************/

#include "ihux.h"

Ihux iHux(1);   // add this class instance for debugging support

/********************************************
 * LOCAL #define CONSTANTS AND MACROS       *
 ********************************************/

#define CTL_TEMPERATURE_RANGE_PLACEHOLDER 0

/********************************************
 * EXPORTED TYPES DEFINITIONS               *
 ********************************************/

enum ModemState_t
{
  MODEM_STATE_INIT_DEVICE,
  MODEM_STATE_DEVICE,
  MODEM_STATE_INIT_NODE,
  MODEM_STATE_NODE,
  MODEM_STATE_UNKNOWN = 0xFF,
};

/********************************************
 * STATIC VARIABLES                         *
 ********************************************/

static const uint8_t ctl_registration[] = {
    lowByte(MESH_MODEL_ID_LIGHT_CTL_SERVER),
    highByte(MESH_MODEL_ID_LIGHT_CTL_SERVER),

    CTL_TEMPERATURE_RANGE_PLACEHOLDER,
    CTL_TEMPERATURE_RANGE_PLACEHOLDER,
    CTL_TEMPERATURE_RANGE_PLACEHOLDER,
    CTL_TEMPERATURE_RANGE_PLACEHOLDER,
};

static const uint8_t lightness_registration[] = {
    lowByte(MESH_MODEL_ID_LIGHT_LC_SERVER),
    highByte(MESH_MODEL_ID_LIGHT_LC_SERVER),
};
/*
static const uint8_t pir_registration[] = {
    lowByte(MESH_MODEL_ID_SENSOR_SERVER),
    highByte(MESH_MODEL_ID_SENSOR_SERVER),

    0x01,   //Number of sensors

    lowByte(MESH_PROPERTY_ID_PRESENCE_DETECTED),
    highByte(MESH_PROPERTY_ID_PRESENCE_DETECTED),
    lowByte(PIR_POSITIVE_TOLERANCE),
    highByte(PIR_POSITIVE_TOLERANCE),
    lowByte(PIR_NEGATIVE_TOLERANCE),
    highByte(PIR_NEGATIVE_TOLERANCE),
    PIR_SAMPLING_FUNCTION,
    PIR_MEASUREMENT_PERIOD,
    PIR_UPDATE_INTERVAL,
};

static const uint8_t als_registration[] = {
    lowByte(MESH_MODEL_ID_SENSOR_SERVER),
    highByte(MESH_MODEL_ID_SENSOR_SERVER),

    0x01,   //Number of sensors

    lowByte(MESH_PROPERTY_ID_PRESENT_AMBIENT_LIGHT_LEVEL),
    highByte(MESH_PROPERTY_ID_PRESENT_AMBIENT_LIGHT_LEVEL),
    lowByte(ALS_POSITIVE_TOLERANCE),
    highByte(ALS_POSITIVE_TOLERANCE),
    lowByte(ALS_NEGATIVE_TOLERANCE),
    highByte(ALS_NEGATIVE_TOLERANCE),
    ALS_SAMPLING_FUNCTION,
    ALS_MEASUREMENT_PERIOD,
    ALS_UPDATE_INTERVAL,
};

static const uint8_t current_voltage_registration[] = {
    lowByte(MESH_MODEL_ID_SENSOR_SERVER),
    highByte(MESH_MODEL_ID_SENSOR_SERVER),

    0x02,   //Number of sensors

    lowByte(MESH_PROPERTY_ID_PRESENT_INPUT_VOLTAGE),
    highByte(MESH_PROPERTY_ID_PRESENT_INPUT_VOLTAGE),
    lowByte(VOLTAGE_SENSOR_POSITIVE_TOLERANCE),
    highByte(VOLTAGE_SENSOR_POSITIVE_TOLERANCE),
    lowByte(VOLTAGE_SENSOR_NEGATIVE_TOLERANCE),
    highByte(VOLTAGE_SENSOR_NEGATIVE_TOLERANCE),
    VOLTAGE_SENSOR_SAMPLING_FUNCTION,
    VOLTAGE_SENSOR_MEASUREMENT_PERIOD,
    VOLTAGE_SENSOR_UPDATE_INTERVAL,

    lowByte(MESH_PROPERTY_ID_PRESENT_INPUT_CURRENT),
    highByte(MESH_PROPERTY_ID_PRESENT_INPUT_CURRENT),
    lowByte(CURRENT_SENSOR_POSITIVE_TOLERANCE),
    highByte(CURRENT_SENSOR_POSITIVE_TOLERANCE),
    lowByte(CURRENT_SENSOR_NEGATIVE_TOLERANCE),
    highByte(CURRENT_SENSOR_NEGATIVE_TOLERANCE),
    CURRENT_SENSOR_SAMPLING_FUNCTION,
    CURRENT_SENSOR_MEASUREMENT_PERIOD,
    CURRENT_SENSOR_UPDATE_INTERVAL,
};

static const uint8_t power_energy_registration[] = {
    lowByte(MESH_MODEL_ID_SENSOR_SERVER),
    highByte(MESH_MODEL_ID_SENSOR_SERVER),

    0x02,   //Number of sensors

    lowByte(MESH_PROPERTY_ID_PRESENT_DEVICE_INPUT_POWER),
    highByte(MESH_PROPERTY_ID_PRESENT_DEVICE_INPUT_POWER),
    lowByte(POWER_SENSOR_POSITIVE_TOLERANCE),
    highByte(POWER_SENSOR_POSITIVE_TOLERANCE),
    lowByte(POWER_SENSOR_NEGATIVE_TOLERANCE),
    highByte(POWER_SENSOR_NEGATIVE_TOLERANCE),
    POWER_SENSOR_SAMPLING_FUNCTION,
    POWER_SENSOR_MEASUREMENT_PERIOD,
    POWER_SENSOR_UPDATE_INTERVAL,

    lowByte(MESH_PROPERTY_ID_TOTAL_DEVICE_ENERGY_USE),
    highByte(MESH_PROPERTY_ID_TOTAL_DEVICE_ENERGY_USE),
    lowByte(ENERGY_SENSOR_POSITIVE_TOLERANCE),
    highByte(ENERGY_SENSOR_POSITIVE_TOLERANCE),
    lowByte(ENERGY_SENSOR_NEGATIVE_TOLERANCE),
    highByte(ENERGY_SENSOR_NEGATIVE_TOLERANCE),
    ENERGY_SENSOR_SAMPLING_FUNCTION,
    ENERGY_SENSOR_MEASUREMENT_PERIOD,
    ENERGY_SENSOR_UPDATE_INTERVAL,
};

static const uint8_t health_registration[] = {
    lowByte(MESH_MODEL_ID_HEALTH_SERVER),
    highByte(MESH_MODEL_ID_HEALTH_SERVER),

    0x01,   //Number of company IDs

    lowByte(SILVAIR_ID),
    highByte(SILVAIR_ID),
};
*/
static ModemState_t ModemState = MODEM_STATE_UNKNOWN;

#if ENABLE_CTL==1 && ENABLE_LC==1
#undef ENABLE_LC
#define ENABLE_LC 0
#endif

static bool       CTLEnabled    = (ENABLE_CTL != 0);
static bool       LCEnabled     = (ENABLE_LC != 0);
static const bool PIRALSEnabled = (ENABLE_PIRALS != 0);
static const bool ENERGYEnabled = (ENABLE_ENERGY != 0);

/********************************************
 * LOCAL FUNCTIONS PROTOTYPES               *
 ********************************************/

/*
 *  Setup debug interface
 */
void SetupDebug(void);

/*
 *  Process Init Device Event command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessEnterInitDevice(uint8_t * p_payload, uint8_t len);

/*
 *  Process Create Instances Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessEnterDevice(uint8_t * p_payload, uint8_t len);

/*
 *  Process Init Node Event command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessEnterInitNode(uint8_t * p_payload, uint8_t len);

/*
 *  Process Start Node Response command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessEnterNode(uint8_t * p_payload, uint8_t len);

/*
 *  Process Mesh Message Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessMeshCommand(uint8_t * p_payload, uint8_t len);

/*
 *  Process Error command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessError(uint8_t * p_payload, uint8_t len);

/*
 *  Process Start Test Request command
 *
 *  @param * p_payload   Command payload
 *  @param len           Payload len
 */
void ProcessStartTest(uint8_t * p_payload, uint8_t len);

/*
 *  Send Firmware Version Set request
 */
void SendFirmwareVersionSetRequest(void);

/*
 *  Process Firmware Version set response
 */
void ProcessFirmwareVersionSetResponse(void);

/*
 *  Main Arduino setup
 */
void setup(void);

/*
 *  Main Arduino loop
 */
void loop(void);

/********************************************
 * FUNCTION DEFINITIONS                     *
 ********************************************/

void SetupDebug(void)
{
  DEBUG_INTERFACE.begin(DEBUG_INTERFACE_BAUDRATE);
  // Waits for debug interface initialization.
  delay(1000);
  
  iHux.trace(0,"SetDebug(): iHux debugging ON\n");
}

void ProcessEnterInitDevice(uint8_t * p_payload, uint8_t len)
{
  INFO("Init Device State.\n");
  ModemState = MODEM_STATE_INIT_DEVICE;

  AttentionStateSet(false);

  iHux.trace(1,"### STEP 4 @ ProcessEnterInitDevice(): ModemState & AttentionState changed\n"); 
 
  if(!Mesh_IsModelAvailable(p_payload, len, MESH_MODEL_ID_LIGHT_CTL_SERVER) && CTLEnabled)
  {
    INFO("Modem does not support Light CTL Server.\n");

    if(Mesh_IsModelAvailable(p_payload, len, MESH_MODEL_ID_LIGHT_LC_SERVER))
    {
      INFO("Disabling CTL activating LC.\n");
      CTLEnabled = false;
      LCEnabled  = true;
    }
  }

  if(!Mesh_IsModelAvailable(p_payload, len, MESH_MODEL_ID_LIGHT_LC_SERVER) && LCEnabled)
  {
    INFO("Modem does not support Light LC Server.\n");
    return;
  }
/*
  if (!Mesh_IsModelAvailable(p_payload, len, MESH_MODEL_ID_SENSOR_SERVER) && (PIRALSEnabled || ENERGYEnabled))
  {
    INFO("Modem does not support Sensor Server.\n");
    return;
  }
*/
  size_t payload_len = 0;

  if(CTLEnabled)
    payload_len += sizeof(ctl_registration);
  else if(LCEnabled)
    payload_len += sizeof(lightness_registration);
/*
  if(PIRALSEnabled)
    payload_len += sizeof(pir_registration) + sizeof(als_registration);

  if(ENERGYEnabled)
    payload_len += sizeof(current_voltage_registration) + sizeof(power_energy_registration);

  payload_len += sizeof(health_registration);
*/
  uint8_t model_ids[payload_len];
  size_t  index = 0;

  iHux.trace(1,"### STEP 5 @ ProcessEnterInitDevice(): buildup order\n"); 

  if(CTLEnabled)
  {
    iHux.trace(1,"          order CTL\n"); 
    memcpy(model_ids + index, ctl_registration, sizeof(ctl_registration));
    index += sizeof(ctl_registration);
  }
  else if(LCEnabled)
  {
    iHux.trace(1,"          order LC\n"); 
    memcpy(model_ids + index, lightness_registration, sizeof(lightness_registration));
    index += sizeof(lightness_registration);
  }
/*
  if(PIRALSEnabled)
  {
    iHux.trace(1,"          order PIR\n"); 
    memcpy(model_ids + index, pir_registration, sizeof(pir_registration));
    index += sizeof(pir_registration);

    iHux.trace(1,"          order ALS\n"); 
    memcpy(model_ids + index, als_registration, sizeof(als_registration));
    index += sizeof(als_registration);
  }

  if(ENERGYEnabled)
  {
    iHux.trace(1,"          order CURRENT/VOLTAGE\n"); 
    memcpy(model_ids + index, current_voltage_registration, sizeof(current_voltage_registration));
    index += sizeof(current_voltage_registration);

    iHux.trace(1,"          order POWER/ENERGY\n"); 
    memcpy(model_ids + index, power_energy_registration, sizeof(power_energy_registration));
    index += sizeof(power_energy_registration);
  }

  iHux.trace(1,"          order HEALTH\n"); 
  memcpy(model_ids + index, health_registration, sizeof(health_registration));
*/
  SendFirmwareVersionSetRequest();

  iHux.trace(1,"### STEP 7 @ ProcessEnterInitDevice(): request to create instances\n"); 
  UART_SendCreateInstancesRequest(model_ids, sizeof(model_ids));
}

void ProcessEnterDevice(uint8_t * p_payload, uint8_t len)
{
  INFO("Device State.\n");
  iHux.trace(1,"### STEP 9 @ ProcessEnterDevice(): enable startup sequence & change MODEM_STATE_DEVICE \n"); 

  EnableStartupSequence();
  ModemState = MODEM_STATE_DEVICE;
}

void ProcessEnterInitNode(uint8_t * p_payload, uint8_t len)
{
  INFO("Init Node State.\n");
  iHux.trace(1,"### STEP 12 @ ProcessEnterInitNode(): process instance IDs\n"); 
  ModemState = MODEM_STATE_INIT_NODE;
  
  AttentionStateSet(false);

  SetLightnessServerIdx(INSTANCE_INDEX_UNKNOWN);
/*
  SetSensorServerPIRIdx(INSTANCE_INDEX_UNKNOWN);
  SetSensorServerALSIdx(INSTANCE_INDEX_UNKNOWN);
  SetSensorServerVoltCurrIdx(INSTANCE_INDEX_UNKNOWN);
  SetSensorServerPowEnergyIdx(INSTANCE_INDEX_UNKNOWN);

  uint8_t sensor_server_model_id_occurency = 0;

  for (size_t index = 0; index < len;)
  {
    uint16_t model_id = ((uint16_t)p_payload[index++]);
    model_id         |= ((uint16_t)p_payload[index++] << 8);

    if (MESH_MODEL_ID_LIGHT_CTL_SERVER == model_id || MESH_MODEL_ID_LIGHT_LC_SERVER == model_id)
    {
      uint16_t current_model_id_instance_index = index/2;
      SetLightnessServerIdx(current_model_id_instance_index);
      iHux.trace(1,"   => LIGHT_LC_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);

      SetLightCTLSupport(model_id == MESH_MODEL_ID_LIGHT_CTL_SERVER);
      if (model_id == MESH_MODEL_ID_LIGHT_CTL_SERVER)
      {
            iHux.trace(1,"   => LIGHT_CTL_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);
      }

    }

    if (model_id == MESH_MODEL_ID_SENSOR_SERVER)
    {
      uint16_t current_model_id_instance_index = index/2;
      sensor_server_model_id_occurency++;

      if (sensor_server_model_id_occurency == PIR_REGISTRATION_ORDER && PIRALSEnabled)
      {
        SetSensorServerPIRIdx(current_model_id_instance_index);
        iHux.trace(1,"   => SENSOR_PIR_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);
      }
      else if (sensor_server_model_id_occurency == ALS_REGISTRATION_ORDER && PIRALSEnabled)
      {
        SetSensorServerALSIdx(current_model_id_instance_index);
        iHux.trace(1,"   => SENSOR_ALS_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);
      }
      else if (sensor_server_model_id_occurency == VOLT_CURR_REGISTRATION_ORDER && ENERGYEnabled)
      {
        SetSensorServerVoltCurrIdx(current_model_id_instance_index);
        iHux.trace(1,"   => SENSOR_VOLT_CURRENT_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);
      }
      else if (sensor_server_model_id_occurency == POW_ENERGY_REGISTRATION_ORDER && ENERGYEnabled)
      {
        SetSensorServerPowEnergyIdx(current_model_id_instance_index);
        iHux.trace(1,"   => SENSOR_POW_ENERGY_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);
      }
    }

    if (model_id == MESH_MODEL_ID_HEALTH_SERVER)
    {
      uint16_t current_model_id_instance_index = index/2;
      SetHealthServerIdx(current_model_id_instance_index);
      iHux.trace(1,"   => HEALTH_SERVER"); DEBUG(" @%d supported!\n", current_model_id_instance_index);
    }
  }

  if (GetLightnessServerIdx() == INSTANCE_INDEX_UNKNOWN && (LCEnabled || CTLEnabled))
  {
    ModemState = MODEM_STATE_UNKNOWN;
    INFO("Light CTL/LC Server model id not found in init node message\n");
    return;
  }

  if (GetSensorServerPIRIdx() == INSTANCE_INDEX_UNKNOWN && PIRALSEnabled)
  {
    ModemState = MODEM_STATE_UNKNOWN;
    INFO("Sensor server (PIR) model id not found in init node message\n");
    return;
  }

  if (GetSensorServerALSIdx() == INSTANCE_INDEX_UNKNOWN && PIRALSEnabled)
  {
    ModemState = MODEM_STATE_UNKNOWN;
    INFO("Sensor server (ALS) model id not found in init node message\n");
    return;
  }

  if (GetSensorServerVoltCurrIdx() == INSTANCE_INDEX_UNKNOWN && ENERGYEnabled)
  {
    ModemState = MODEM_STATE_UNKNOWN;
    INFO("Sensor server (Voltage Current) model id not found in init node message\n");
    return;
  }

  if (GetSensorServerPowEnergyIdx() == INSTANCE_INDEX_UNKNOWN && ENERGYEnabled)
  {
    ModemState = MODEM_STATE_UNKNOWN;
    INFO("Sensor server (Power Energy) model id not found in init node message\n");
    return;
  }

   if (GetHealthServerIdx() == INSTANCE_INDEX_UNKNOWN)
   {
     ModemState = MODEM_STATE_UNKNOWN;
     INFO("Health Server model id not found in init node message\n");
     return;
   }
*/
  iHux.trace(1,"### STEP 13 @ ProcessEnterInitNode(): SendFirmwareVersion\n"); 
  SendFirmwareVersionSetRequest();

  iHux.trace(1,"### STEP 14 @ ProcessEnterInitNode(): StartNodeRequest\n"); 
  UART_StartNodeRequest();
}

void ProcessEnterNode(uint8_t * p_payload, uint8_t len)
{
  INFO("Node State.\n");
  iHux.trace(1,"### STEP 15 @ ProcessEnterNode(): ModemState = MODEM_STATE_NODE\n"); 

  ModemState = MODEM_STATE_NODE;

  iHux.trace(1,"### STEP 16 @ ProcessEnterNode(): SynchronizeLightness() ...\n"); 
  SynchronizeLightness();
}

void ProcessMeshCommand(uint8_t * p_payload, uint8_t len)
{
  Mesh_ProcessMeshCommand(p_payload, len);
}
 
void ProcessError(uint8_t * p_payload, uint8_t len)
{
  INFO("Error %d\n\n.", p_payload[0]);
}

void SendFirmwareVersionSetRequest(void)
{
  const char * p_firmware_version = BUILD_NUMBER;

  iHux.trace(1,"### STEP 6 @ SendFirmwareVersionSetRequest()\n"); 
  UART_SendFirmwareVersionSetRequest((uint8_t *)p_firmware_version, strlen(p_firmware_version));
}

void ProcessFirmwareVersionSetResponse(void)
{
  iHux.trace(1,"### STEP 8 @ process(): ProcessFirmwareVersionSetResponse()\n"); 
}

void setup(void)
{
  SetupDebug();
  INFO("Server Sample.\n");

  iHux.trace(1,"SetupAttention() ...\n");
  SetupAttention();
/*
  iHux.trace(1,"SetupHealth() ...\n");
  SetupHealth();
*/
  if(LCEnabled || CTLEnabled) 
  {  
    iHux.trace(1,"SetupLightnessServer() ...\n");
    SetupLightnessServer();
  }
/*  
  if(PIRALSEnabled)
  {
    iHux.trace(1,"SetupSensorServer() ...\n");
    SetupSensorServer();
  }
  
  if(ENERGYEnabled)
  {
    iHux.trace(1,"SetupSDM() ...\n");
    SetupSDM();
  }
*/
  UART_Init();
  iHux.trace(1,"### STEP 1 @ setup(): SendSoftwareResetRequest()\n"); 
  UART_SendSoftwareResetRequest();
/*
  iHux.trace(1,"SetupDFU() ...\n");
  SetupDFU();
*/  
  iHux.trace(1,"setup(): setup complete()\n"); 
}

void loop(void)
{
  UART_ProcessIncomingCommand();
/*
  if (!MCU_DFU_IsInProgress())
  {
    LoopHealth();
    if (!IsTestInProgress())
    {
      // Health and Attention shares Status LED
*/      
      LoopAttention();
/*      
    }
*/
    LoopLightnessServer();
/*    
    LoopSDM();

    if (MODEM_STATE_NODE == ModemState)
    {
      LoopSensorSever();
    }
  }
*/  
}
