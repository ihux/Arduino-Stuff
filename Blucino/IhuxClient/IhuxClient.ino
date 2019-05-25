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
#include "LCD.h"
#include "MCU_Sensor.h"
#include "MCU_Switch.h"
#include "MCU_DFU.h"
#include "MCU_Attention.h"
#include "Mesh.h"
#include "UART.h"
#include <limits.h>
#include <string.h>
#include "MCU_Definitions.h"

//==================================================================================================
// iHux tracing support
//==================================================================================================

#include "ihux.h"

    Ihux iHux(1);   // add this class instance for debugging support

//==================================================================================================
// static variables
//==================================================================================================

    static ModemState_t ModemState        = MODEM_STATE_UNKNOWN;
    static bool         LastDfuInProgress = false;

//==================================================================================================
// common function definitions
//==================================================================================================

    static void InfoAboutState(const char *pMsg)
    {
        INFO("                   ===========================================================\n");
        INFO("                                    %s\n",pMsg);
        INFO("                   ===========================================================\n");
    }
    
    void SetupDebug(void)
    {
        DEBUG_INTERFACE.begin(DEBUG_INTERFACE_BAUDRATE);
        // Waits for debug interface initialization.
        delay(1000);
    }

//==================================================================================================
// enter INIT_DEVICE state
//==================================================================================================

    void ProcessEnterInitDevice(uint8_t * p_payload, uint8_t len)
    {
        InfoAboutState("Init Device State");
        ModemState = MODEM_STATE_INIT_DEVICE;
        LCD_UpdateModemState(ModemState);
        AttentionStateSet(false);
      
        iHux.trace(1,"### STEP 4 @ ProcessEnterInitDevice(): ModemState & AttentionState changed\n"); 
       
        SetInstanceIdxSwitch1(INSTANCE_INDEX_UNKNOWN);
        SetInstanceIdxSwitch2(INSTANCE_INDEX_UNKNOWN);
        SetInstanceIdxSensor(INSTANCE_INDEX_UNKNOWN);
      
        if (!Mesh_IsModelAvailable(p_payload, len, MESH_MODEL_ID_LIGHT_LC_CLIENT))
        {
            INFO("Modem does not support Light Lightness Controler Client.\n");
            return;
        }
      
        if (!Mesh_IsModelAvailable(p_payload, len, MESH_MODEL_ID_SENSOR_CLIENT))
        {
            INFO("Modem does not support Sensor Client.\n");
            return;
        }
      
        iHux.trace(1,"### STEP 5 @ ProcessEnterInitDevice(): buildup order\n"); 
      
        uint8_t model_ids[] =
        {
            // First Light Lightness controller client
            lowByte(MESH_MODEL_ID_LIGHT_LC_CLIENT),
            highByte(MESH_MODEL_ID_LIGHT_LC_CLIENT),
            // Second Light Lightness controller client
            lowByte(MESH_MODEL_ID_LIGHT_LC_CLIENT),
            highByte(MESH_MODEL_ID_LIGHT_LC_CLIENT),
            // Sensor client
            lowByte(MESH_MODEL_ID_SENSOR_CLIENT),
            highByte(MESH_MODEL_ID_SENSOR_CLIENT),
        };
      
        iHux.trace(1,"          order 1st LC Client\n"); 
        iHux.trace(1,"          order 2nd LC Client\n"); 
        iHux.trace(1,"          order Sensor Client\n"); 
       
        SendFirmwareVersionSetRequest();// iHux ### STEP 6 (see inside function!)
      
        iHux.trace(1,"### STEP 7 @ ProcessEnterInitDevice(): request to create instances\n"); 
        UART_SendCreateInstancesRequest(model_ids, sizeof(model_ids));
      
        iHux.trace(1,"### STEP 7A @ UART_ModemFirmwareVersionRequest()\n"); 
        UART_ModemFirmwareVersionRequest();
    }

//==================================================================================================
// enter DEVICE state
//==================================================================================================

    void ProcessEnterDevice(uint8_t * p_payload, uint8_t len)
    {
        InfoAboutState("   Device State");
        iHux.trace(1,"### STEP 9 @ ProcessEnterDevice(): change MODEM_STATE_DEVICE \n"); 
      
        // EnableStartupSequence();        // iHux: only MCU Server has Enable StasrtupSequence() call
        ModemState = MODEM_STATE_DEVICE;
        LCD_UpdateModemState(ModemState);  // iHux: only client has LCD_UpdateModemState()
    }

//==================================================================================================
// enter INIT-NODE state
//==================================================================================================

    void ProcessEnterInitNode(uint8_t * p_payload, uint8_t len)
    {
        InfoAboutState("Init Node State");
      
        iHux.trace(1,"### STEP 12 @ ProcessEnterInitNode(): process instance IDs\n"); 
      
        ModemState = MODEM_STATE_INIT_NODE;
        LCD_UpdateModemState(ModemState);
        AttentionStateSet(false);
      
        SetInstanceIdxSwitch1(INSTANCE_INDEX_UNKNOWN);
        SetInstanceIdxSwitch2(INSTANCE_INDEX_UNKNOWN);
        SetInstanceIdxSensor(INSTANCE_INDEX_UNKNOWN);
      
        for (size_t index = 0; index < len;)
        {
            uint16_t model_id = ((uint16_t)p_payload[index++]);
            model_id         |= ((uint16_t)p_payload[index++] << 8);
            uint16_t current_model_id_instance_index = index/2;
        
            if (MESH_MODEL_ID_LIGHT_LC_CLIENT == model_id)
            {
                if (GetInstanceIdxSwitch1() == INSTANCE_INDEX_UNKNOWN)
                {
                    SetInstanceIdxSwitch1(current_model_id_instance_index);
                }
                else
                {
                    SetInstanceIdxSwitch2(current_model_id_instance_index);
                }
            }
        
            if (MESH_MODEL_ID_SENSOR_CLIENT == model_id)
            {
                SetInstanceIdxSensor(current_model_id_instance_index);
            }
        }
      
        if (GetInstanceIdxSwitch1() == INSTANCE_INDEX_UNKNOWN)
        {
            ModemState = MODEM_STATE_UNKNOWN;
            LCD_UpdateModemState(ModemState);
            INFO("First Light Lightness Controller Client model id not found in init node message\n");
            return;
        }
      
        if (GetInstanceIdxSwitch2() == INSTANCE_INDEX_UNKNOWN)
        {
            ModemState = MODEM_STATE_UNKNOWN;
            LCD_UpdateModemState(ModemState);
            INFO("Second Light Lightness Controller Client model id not found in init node message\n");
            return;
        }
      
        if (GetInstanceIdxSensor() == INSTANCE_INDEX_UNKNOWN)
        {
            ModemState = MODEM_STATE_UNKNOWN;
            LCD_UpdateModemState(ModemState);
            INFO("Sensor Client model id not found in init node message\n");
            return;
        }
      
        iHux.trace(1,"### STEP 13 @ ProcessEnterInitNode(): SendFirmwareVersion\n"); 
        SendFirmwareVersionSetRequest();
      
        iHux.trace(1,"### STEP 14 @ ProcessEnterInitNode(): StartNodeRequest\n"); 
        UART_StartNodeRequest();
      
        iHux.trace(1,"### STEP 14A @ ProcessEnterInitNode(): UART_ModemFirmwareVersionRequest\n");  // NOTE iHux: only MCU Client 
        UART_ModemFirmwareVersionRequest();
    }
    
//==================================================================================================
// enter INIT-NODE state
//==================================================================================================

    void ProcessEnterNode(uint8_t * p_payload, uint8_t len)
    {
        InfoAboutState("  Node State");
      
        iHux.trace(1,"### STEP 15 @ ProcessEnterNode(): ModemState = MODEM_STATE_NODE\n"); 
        
        ModemState = MODEM_STATE_NODE;
        
        // iHux.trace(1,"### STEP 16 @ ProcessEnterNode(): SynchronizeLightness() ...\n"); 
        // SynchronizeLightness();  NOTE iHux: only for MCU Server()
      
        iHux.trace(1,"### STEP 16 @ ProcessEnterNode(): LCD_UpdateModemState() ...\n"); 
        LCD_UpdateModemState(ModemState);
    }

//==================================================================================================
// process mresh command
//==================================================================================================

    void ProcessMeshCommand(uint8_t * p_payload, uint8_t len)
    {
        Mesh_ProcessMeshCommand(p_payload, len);
    }

//==================================================================================================
// error processing
//==================================================================================================

    void ProcessError(uint8_t * p_payload, uint8_t len)
    {
        INFO("Error %d\n\n.", p_payload[0]);
    }
    
//==================================================================================================
// firmware
//==================================================================================================

    void ProcessModemFirmwareVersion(uint8_t * p_payload, uint8_t len)
    {
        iHux.trace(1,"### STEP 8A @ ProcessModemFirmwareVersion()\n"); 
        INFO("                   Process Modem Firmware Version\n");
        LCD_UpdateModemFwVersion((char*)p_payload, len);
    }
    
    void SendFirmwareVersionSetRequest(void)
    {
        const char * p_firmware_version = BUILD_NUMBER;
        iHux.trace(1,"### STEP 6 @ SendFirmwareVersionSetRequest()\n"); 
        UART_SendFirmwareVersionSetRequest((uint8_t *)p_firmware_version, strlen(p_firmware_version));
    }
    
    void ProcessFirmwareVersionSetResponse(void)  // iHux: added
    {
        iHux.trace(1,"### STEP 8 @ process(): ProcessFirmwareVersionSetResponse()\n"); 
    }
    
    void ProcessFactoryResetEvent(void)
    {
        LCD_EraseSensorsValues();
    }
    
    void ProcessStartTest(uint8_t * p_payload, uint8_t len)   // iHux - copied for compatibility
    {
        iHux.trace(1,"ProcessStartTrst() - empty function for MCUiHuxClient!\n");
        /*
        UART_SendTestStartResponse(NULL, 0);
        digitalWrite(PIN_LED_STATUS, true);
        memcpy(TestStartPayload, p_payload, len);
        TestStarted        = true;
        TestStartTimestamp = millis();
        */
    }

    bool IsDfuStateChanged(void)
    {
        return LastDfuInProgress ^ MCU_DFU_IsInProgress();
    }
    
//==================================================================================================
// setup
//==================================================================================================

    void setup()
    {
        SetupDebug();
      
        iHux.trace(1,"SetupAttention() ...\n");
        SetupAttention();
        
        iHux.trace(1,"LCD_Setup() ...\n");
        LCD_Setup();
      
        iHux.trace(1,"SetupSwitch() ...\n");
        SetupSwitch();
      
        iHux.trace(1,"SetupSensor() ...\n");
        SetupSensor();
      
        iHux.trace(1,"UART_Init() ...\n");
        UART_Init();
        
        iHux.trace(1,"### STEP 1 @ setup(): SendSoftwareResetRequest()\n"); 
        UART_SendSoftwareResetRequest();
      
        iHux.trace(1,"SetupDFU() ...\n");
        SetupDFU();
    }
    
    void loop()
    {
        Mesh_Loop();
        LCD_Loop();
        LoopAttention();
        UART_ProcessIncomingCommand();
      
        switch (ModemState)
        {
            case MODEM_STATE_UNKNOWN:
            case MODEM_STATE_INIT_DEVICE:
            case MODEM_STATE_INIT_NODE:
              break;
        
            case MODEM_STATE_DEVICE:
            case MODEM_STATE_NODE:
      
                // DFU (Device Firmware Upgrade)
                
            if (IsDfuStateChanged())
            {
              LastDfuInProgress = MCU_DFU_IsInProgress();
              LCD_UpdateDfuState(LastDfuInProgress);
            }
      
            if (!MCU_DFU_IsInProgress())
            {
              LoopSwitch();
            }
            break;
        }
    }
