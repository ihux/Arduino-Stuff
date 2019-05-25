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

#include "LCD.h"
#include "Arduino.h"
#include "Config.h"
#include "UART.h"
//#include "LiquidCrystal_I2C.h"

/********************************************
 * LOCAL #define CONSTANTS AND MACROS       *
 ********************************************/

#define LCD_SCREEN_SWITCH_INTV_MS          5000           /**< Defines LCD screen switch interval. */
#define LCD_ROWS_NUMBER                    4              /**< Defines number of LCD rows. */
#define LCD_COLUMNS_NUMBER                 20             /**< Defines number of LCD columns. */

#define LCD_PIR_VALUE_EXP_MS               60000          /**< PIR Sensor value expiration time in milliseconds. */
#define LCD_ALS_VALUE_EXP_MS               60000          /**< ALS Sensor value expiration time in milliseconds. */
#define LCD_POWER_VALUE_EXP_MS             60000          /**< Power Sensor value expiration time in milliseconds. */
#define LCD_CURRENT_VALUE_EXP_MS           60000          /**< Current Sensor value expiration time in milliseconds. */
#define LCD_VOLTAGE_VALUE_EXP_MS           60000          /**< Voltage Sensor value expiration time in milliseconds. */
#define LCD_ENERGY_VALUE_EXP_MS            60000          /**< Energy Sensor value expiration time in milliseconds. */

/********************************************
 * LOCAL TYPES DEFINITIONS                  *
 ********************************************/

 typedef enum
 {
    SCREEN_TYPE_FIRST,
    SCREEN_TYPE_MODEM_STATE_PIR_ALS = SCREEN_TYPE_FIRST,
    SCREEN_TYPE_ENERGY_SENSORS,
    SCREEN_TYPE_FW_VERSION,
    SCREEN_TYPE_DFU,
    SCREEN_TYPE_LAST  = SCREEN_TYPE_DFU,
 } ScreenType_T;

/********************************************
 * STATIC VARIABLES                         *
 ********************************************/

static ModemState_t  LCD_ModemState                         = MODEM_STATE_UNKNOWN;

#ifdef LCD_SUPPORTED
static LiquidCrystal_I2C Lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
static bool          LCD_DfuInProgress                      = false;
static char          LCD_ModemFwVersion[LCD_COLUMNS_NUMBER] = "Unknown";
static ScreenType_T  LCD_CurrentScreen                      = SCREEN_TYPE_FIRST;
static unsigned long LCD_CurrentScreenTimestamp             = 0;
static bool          LCD_NeedsUpdate                        = false;
#endif

typedef enum
{
  SENSOR_VALUE_UNKNOWN,
  SENSOR_VALUE_ACTUAL,
  SENSOR_VALUE_EXPIRED,
} LCD_SensorValueState_T;

typedef struct
{
  SensorProperty_T       property;
  SensorValue_T          value;
  unsigned long          value_timestamp;
  unsigned long          value_expiration_time;
  LCD_SensorValueState_T value_state;
} LCD_Sensor_T;

#ifdef LCD_SUPPORTED
static LCD_Sensor_T LCD_PirSensor = {
  .property              = PRESENCE_DETECTED,
  .value =
  {
    .pir                 = 0,
  },
  .value_timestamp       = 0,
  .value_expiration_time = LCD_PIR_VALUE_EXP_MS,
  .value_state           = SENSOR_VALUE_UNKNOWN,
};

static LCD_Sensor_T LCD_AlsSensor = {
  .property              = PRESENT_AMBIENT_LIGHT_LEVEL,
  .value =
  {
    .als                 = 0,
  },
  .value_timestamp       = 0,
  .value_expiration_time = LCD_ALS_VALUE_EXP_MS,
  .value_state           = SENSOR_VALUE_UNKNOWN,
};

static LCD_Sensor_T LCD_PowerSensor = {
  .property              = PRESENT_DEVICE_INPUT_POWER,
  .value =
  {
    .power               = 0,
  },
  .value_timestamp       = 0,
  .value_expiration_time = LCD_POWER_VALUE_EXP_MS,
  .value_state           = SENSOR_VALUE_UNKNOWN,
};

static LCD_Sensor_T LCD_CurrentSensor = {
  .property              = PRESENT_INPUT_CURRENT,
  .value =
  {
    .current             = 0,
  },
  .value_timestamp       = 0,
  .value_expiration_time = LCD_CURRENT_VALUE_EXP_MS,
  .value_state           = SENSOR_VALUE_UNKNOWN,
};

static LCD_Sensor_T LCD_VoltageSensor = {
  .property              = PRESENT_INPUT_VOLTAGE,
  .value =
  {
    .voltage             = 0,
  },
  .value_timestamp       = 0,
  .value_expiration_time = LCD_VOLTAGE_VALUE_EXP_MS,
  .value_state           = SENSOR_VALUE_UNKNOWN,
};

static LCD_Sensor_T LCD_EnergySensor = {
  .property              = TOTAL_DEVICE_ENERGY_USE,
  .value =
  {
    .energy              = 0,
  },
  .value_timestamp       = 0,
  .value_expiration_time = LCD_ENERGY_VALUE_EXP_MS,
  .value_state           = SENSOR_VALUE_UNKNOWN,
};
#endif

/********************************************
 * LOCAL FUNCTIONS PROTOTYPES               *
 ********************************************/

/*
 *  Display line of text on LCD screen
 */
#ifdef LCD_SUPPORTED
static void DisplayLine(size_t line, const char * text);

/*
 *  Display Modem State on LCD screen
 */
static void DisplayModemState(uint8_t lineNumber, ModemState_t modemState);

/*
 *  Display LCD screen
 */
static void DisplayScreen(uint8_t screenNum);

/*
 *  Switch to another Screen
 */
static void ScreenIterate(void);

/*
 *  Check if Sensor values expired
 */
static void CheckSensorValuesExpiration(void);
#endif

/********************************************
 * EXPORTED FUNCTION DEFINITIONS            *
 ********************************************/

void LCD_Setup(void)
{
#ifdef LCD_SUPPORTED
  Lcd.begin(LCD_COLUMNS_NUMBER, LCD_ROWS_NUMBER);

  DisplayScreen(LCD_CurrentScreen);
#endif
}


void LCD_UpdateModemState(ModemState_t modemState)
{
  #ifdef LCD_SUPPORTED  // HACK iHux
  if (LCD_ModemState != modemState
    && LCD_CurrentScreen == SCREEN_TYPE_MODEM_STATE_PIR_ALS) LCD_NeedsUpdate = true;
  #endif                // HACK iHux

  LCD_ModemState = modemState;
}


void LCD_UpdateModemFwVersion(char * fwVersion, uint8_t fwVerLen)
{
  #ifdef LCD_SUPPORTED
  if (fwVerLen > LCD_COLUMNS_NUMBER) fwVerLen = LCD_COLUMNS_NUMBER;

  if (strncmp(LCD_ModemFwVersion, fwVersion, fwVerLen)
    && LCD_CurrentScreen == SCREEN_TYPE_FW_VERSION) LCD_NeedsUpdate = true;
  strncpy(LCD_ModemFwVersion, fwVersion, fwVerLen);
  #endif
}


void LCD_UpdateSensorValue(SensorProperty_T sensorProperty, SensorValue_T sensorValue)
{
  #ifdef LCD_SUPPORTED
  switch (sensorProperty)
  {
    case PRESENCE_DETECTED:
    {
      if (LCD_PirSensor.value.pir != sensorValue.pir
        && LCD_CurrentScreen == SCREEN_TYPE_MODEM_STATE_PIR_ALS) LCD_NeedsUpdate = true;

      LCD_PirSensor.value.pir       = sensorValue.pir;
      LCD_PirSensor.value_state     = SENSOR_VALUE_ACTUAL;
      LCD_PirSensor.value_timestamp = millis();
      break;
    }
    case PRESENT_AMBIENT_LIGHT_LEVEL:
    {
      if (LCD_AlsSensor.value.als != sensorValue.als
        && LCD_CurrentScreen == SCREEN_TYPE_MODEM_STATE_PIR_ALS) LCD_NeedsUpdate = true;

      LCD_AlsSensor.value.als       = sensorValue.als;
      LCD_AlsSensor.value_state     = (sensorValue.als == MESH_PROPERTY_PRESENT_AMBIENT_LIGHT_LEVEL_UNKNOWN_VAL)
                                      ? SENSOR_VALUE_UNKNOWN
                                      : SENSOR_VALUE_ACTUAL;
      LCD_AlsSensor.value_timestamp = millis();
      break;
    }
    case PRESENT_DEVICE_INPUT_POWER:
    {
      if (LCD_PowerSensor.value.power != sensorValue.power
        && LCD_CurrentScreen == SCREEN_TYPE_ENERGY_SENSORS) LCD_NeedsUpdate = true;

      LCD_PowerSensor.value.power     = sensorValue.power;
      LCD_PowerSensor.value_state     = (sensorValue.power == MESH_PROPERTY_PRESENT_DEVICE_INPUT_POWER_UNKNOWN_VAL)
                                        ? SENSOR_VALUE_UNKNOWN
                                        : SENSOR_VALUE_ACTUAL;
      LCD_PowerSensor.value_timestamp = millis();
      break;
    }
    case PRESENT_INPUT_CURRENT:
    {
      if (LCD_CurrentSensor.value.current != sensorValue.current
        && LCD_CurrentScreen == SCREEN_TYPE_ENERGY_SENSORS) LCD_NeedsUpdate = true;

      LCD_CurrentSensor.value.current   = sensorValue.current;
      LCD_CurrentSensor.value_state     = (sensorValue.current == MESH_PROPERTY_PRESENT_INPUT_CURRENT_UNKNOWN_VAL)
                                          ? SENSOR_VALUE_UNKNOWN
                                          : SENSOR_VALUE_ACTUAL;
      LCD_CurrentSensor.value_timestamp = millis();
      break;
    }
    case PRESENT_INPUT_VOLTAGE:
    {
      if (LCD_VoltageSensor.value.voltage != sensorValue.voltage
        && LCD_CurrentScreen == SCREEN_TYPE_ENERGY_SENSORS) LCD_NeedsUpdate = true;

      LCD_VoltageSensor.value.voltage   = sensorValue.voltage;
      LCD_VoltageSensor.value_state     = (sensorValue.voltage == MESH_PROPERTY_PRESENT_INPUT_VOLTAGE_UNKNOWN_VAL)
                                          ? SENSOR_VALUE_UNKNOWN
                                          : SENSOR_VALUE_ACTUAL;
      LCD_VoltageSensor.value_timestamp = millis();
      break;
    }
    case TOTAL_DEVICE_ENERGY_USE:
    {
      if (LCD_EnergySensor.value.energy != sensorValue.energy
        && LCD_CurrentScreen == SCREEN_TYPE_ENERGY_SENSORS) LCD_NeedsUpdate = true;

      LCD_EnergySensor.value.energy    = sensorValue.energy;
      LCD_EnergySensor.value_state     = (sensorValue.energy == MESH_PROPERTY_TOTAL_DEVICE_ENERGY_USE_UNKNOWN_VAL)
                                        ? SENSOR_VALUE_UNKNOWN
                                        : SENSOR_VALUE_ACTUAL;
      LCD_EnergySensor.value_timestamp = millis();
      break;
    }
  }
  #endif
}

void LCD_UpdateDfuState(bool dfuInProgress)
{
  #ifdef LCD_SUPPORTED
  LCD_DfuInProgress = dfuInProgress;
  #endif
}


void LCD_Loop(void)
{
  #ifdef LCD_SUPPORTED

  bool switchScreen = (millis() >= (LCD_CurrentScreenTimestamp + LCD_SCREEN_SWITCH_INTV_MS));
  CheckSensorValuesExpiration();

  if (switchScreen)                   ScreenIterate();
  if (switchScreen | LCD_NeedsUpdate) DisplayScreen(LCD_CurrentScreen);

#endif
}


void LCD_Reinit(void)
{
  #ifdef LCD_SUPPORTED
  Lcd.begin(LCD_COLUMNS_NUMBER, LCD_ROWS_NUMBER);
  LCD_NeedsUpdate = true;
  #endif
}

void LCD_EraseSensorsValues(void)
{
#ifdef LCD_SUPPORTED
  LCD_PirSensor.value_state     = SENSOR_VALUE_UNKNOWN;
  LCD_AlsSensor.value_state     = SENSOR_VALUE_UNKNOWN;
  LCD_PowerSensor.value_state   = SENSOR_VALUE_UNKNOWN;
  LCD_CurrentSensor.value_state = SENSOR_VALUE_UNKNOWN;
  LCD_VoltageSensor.value_state = SENSOR_VALUE_UNKNOWN;
  LCD_EnergySensor.value_state  = SENSOR_VALUE_UNKNOWN;

  LCD_NeedsUpdate               = true;
#endif
}

/********************************************
 * LOCAL FUNCTION DEFINITIONS               *
 *******************************************/

#ifdef LCD_SUPPORTED
static void DisplayLine(size_t line, const char * text)
{
  if (strlen(text) > LCD_COLUMNS_NUMBER)
  {
    INFO("Trying to write too long string on LCD: %s", text);
    return;
  }

  Lcd.setCursor(0, line);
  Lcd.print(text);
}

static void DisplayScreen(uint8_t screenNum)
{
  switch(screenNum)
  {
    case SCREEN_TYPE_DFU:
    {
      Lcd.clear();

      if (LCD_DfuInProgress)
      {
        DisplayLine(0, "DFU in progress");
      }

      break;
    }

    case SCREEN_TYPE_MODEM_STATE_PIR_ALS:
    {
      char text[LCD_COLUMNS_NUMBER] = {0};

      Lcd.clear();

      DisplayModemState(0, LCD_ModemState);

      strcpy(text, "ALS: ");
      if (LCD_AlsSensor.value_state != SENSOR_VALUE_UNKNOWN)
      {
        if (LCD_AlsSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), "(");
        itoa(LCD_AlsSensor.value.als / 100, text + strlen(text), 10);
        strcpy(text + strlen(text), ".00");
        itoa(LCD_AlsSensor.value.als % 100, text + strlen(text) - (LCD_AlsSensor.value.als % 100 < 10 ? 1 : 2), 10);
        strcpy(text + strlen(text), " lux");
        if (LCD_AlsSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), ")");
      }
      else
      {
        strcpy(text + strlen(text), "Unknown");
      }
      DisplayLine(2, text);

      strcpy(text, "PIR: ");
      if (LCD_PirSensor.value_state != SENSOR_VALUE_UNKNOWN)
      {
        if (LCD_PirSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), "(");
        LCD_PirSensor.value.pir ? strcpy(text + strlen(text), "True") : strcpy(text + strlen(text), "False");
        if (LCD_PirSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), ")");
      }
      else
      {
        strcpy(text + strlen(text), "Unknown");
      }
      DisplayLine(3, text);

      break;
    }

    case SCREEN_TYPE_ENERGY_SENSORS:
    {
      char text[LCD_COLUMNS_NUMBER] = {0};

      Lcd.clear();

      strcpy(text, "Power:   ");
      if (LCD_PowerSensor.value_state != SENSOR_VALUE_UNKNOWN)
      {
        if (LCD_PowerSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), "(");
        itoa(LCD_PowerSensor.value.power / 10, text + strlen(text), 10);
        strcpy(text + strlen(text), ".");
        itoa(LCD_PowerSensor.value.power % 10, text + strlen(text), 10);
        strcpy(text + strlen(text), " W");
        if (LCD_PowerSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), ")");
      }
      else
      {
        strcpy(text + strlen(text), "Unknown");
      }
      DisplayLine(0, text);

      strcpy(text, "Energy:  ");
      if (LCD_EnergySensor.value_state != SENSOR_VALUE_UNKNOWN)
      {
        if (LCD_EnergySensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), "(");
        itoa(LCD_EnergySensor.value.energy, text + strlen(text), 10);
        strcpy(text + strlen(text), " kWh");
        if (LCD_EnergySensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), ")");
      }
      else
      {
        strcpy(text + strlen(text), "Unknown");
      }
      DisplayLine(1, text);

      strcpy(text, "Voltage: ");
      if (LCD_VoltageSensor.value_state != SENSOR_VALUE_UNKNOWN)
      {
        if (LCD_VoltageSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), "(");
        itoa(LCD_VoltageSensor.value.voltage / 64, text + strlen(text), 10);
        strcpy(text + strlen(text), ".00");
        itoa(LCD_VoltageSensor.value.voltage % 64, text + strlen(text) - (LCD_VoltageSensor.value.voltage % 64 < 10 ? 1 : 2), 10);
        strcpy(text + strlen(text), " V");
        if (LCD_VoltageSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), ")");
      }
      else
      {
        strcpy(text + strlen(text), "Unknown");
      }
      DisplayLine(2, text);

      strcpy(text, "Current: ");
      if (LCD_CurrentSensor.value_state != SENSOR_VALUE_UNKNOWN)
      {
        if (LCD_CurrentSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), "(");
        itoa(LCD_CurrentSensor.value.current / 100, text + strlen(text), 10);
        strcpy(text + strlen(text), ".00");
        itoa(LCD_CurrentSensor.value.current % 100, text + strlen(text) - (LCD_CurrentSensor.value.current % 100 < 10 ? 1 : 2), 10);
        strcpy(text + strlen(text), " A");
        if (LCD_CurrentSensor.value_state == SENSOR_VALUE_EXPIRED) strcpy(text + strlen(text), ")");
      }
      else
      {
        strcpy(text + strlen(text), "Unknown");
      }
      DisplayLine(3, text);

      break;
    }

    case SCREEN_TYPE_FW_VERSION:
    {
      Lcd.clear();

      DisplayLine(0, "Modem FW version");
      DisplayLine(1, LCD_ModemFwVersion);
      DisplayLine(2, "MCU FW version");
      DisplayLine(3, BUILD_NUMBER);

      break;
    }
  }

  LCD_NeedsUpdate = false;
}

static void DisplayModemState(uint8_t lineNumber, ModemState_t modemState)
{
  char const * modem_states[] =
  {
    "Init Device state",
    "Device state",
    "Init Node state",
    "Node state",
    "Unknown state",
  };

  DisplayLine(lineNumber, modem_states[modemState]);
}

static void ScreenIterate(void)
{
  LCD_CurrentScreen = (ScreenType_T)(LCD_CurrentScreen + 1);
  if (LCD_CurrentScreen == SCREEN_TYPE_DFU && !LCD_DfuInProgress) LCD_CurrentScreen = (ScreenType_T)(LCD_CurrentScreen + 1);
  if (LCD_CurrentScreen > SCREEN_TYPE_LAST) LCD_CurrentScreen = SCREEN_TYPE_FIRST;
  if (LCD_DfuInProgress) LCD_CurrentScreen = SCREEN_TYPE_DFU;

  LCD_CurrentScreenTimestamp = millis();
}

static void CheckSensorValuesExpiration(void)
{
  if (millis() > LCD_PirSensor.value_timestamp + LCD_PirSensor.value_expiration_time
    && LCD_PirSensor.value_state == SENSOR_VALUE_ACTUAL)
  {
    LCD_PirSensor.value_state = SENSOR_VALUE_EXPIRED;
    LCD_NeedsUpdate           = true;
  }

  if (millis() > LCD_AlsSensor.value_timestamp + LCD_AlsSensor.value_expiration_time
    && LCD_AlsSensor.value_state == SENSOR_VALUE_ACTUAL)
  {
    LCD_AlsSensor.value_state = SENSOR_VALUE_EXPIRED;
    LCD_NeedsUpdate           = true;
  }

  if (millis() > LCD_PowerSensor.value_timestamp + LCD_PowerSensor.value_expiration_time
    && LCD_PowerSensor.value_state == SENSOR_VALUE_ACTUAL)
  {
    LCD_PowerSensor.value_state = SENSOR_VALUE_EXPIRED;
    LCD_NeedsUpdate             = true;
  }

  if (millis() > LCD_CurrentSensor.value_timestamp + LCD_CurrentSensor.value_expiration_time
    && LCD_CurrentSensor.value_state == SENSOR_VALUE_ACTUAL)
  {
    LCD_CurrentSensor.value_state = SENSOR_VALUE_EXPIRED;
    LCD_NeedsUpdate               = true;
  }

  if (millis() > LCD_VoltageSensor.value_timestamp + LCD_VoltageSensor.value_expiration_time
    && LCD_VoltageSensor.value_state == SENSOR_VALUE_ACTUAL)
  {
    LCD_VoltageSensor.value_state = SENSOR_VALUE_EXPIRED;
    LCD_NeedsUpdate               = true;
  }

  if (millis() > LCD_EnergySensor.value_timestamp + LCD_EnergySensor.value_expiration_time
    && LCD_EnergySensor.value_state == SENSOR_VALUE_ACTUAL)
  {
    LCD_EnergySensor.value_state = SENSOR_VALUE_EXPIRED;
    LCD_NeedsUpdate              = true;
  }
}
#endif
