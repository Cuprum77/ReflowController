#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


// MCP9600 Registers
#define MCP9600_HOT_JUNCTION_TEMP           0x00
#define MCP9600_DELTA_TEMP                  0x01
#define MCP9600_COLD_JUNCTION_TEMP          0x02
#define MCP9600_RAW_ADC_DATA                0x03
#define MCP9600_STATUS                      0x04
#define MCP9600_THERMOCOUPLE_SENSOR_CONFIG  0x05
#define MCP9600_DEVICE_CONFIG               0x06
#define MCP9600_ALERT1_CONFIG               0x08
#define MCP9600_ALERT2_CONFIG               0x09
#define MCP9600_ALERT3_CONFIG               0x0A
#define MCP9600_ALERT4_CONFIG               0x0B
#define MCP9600_ALERT1_HYSTERESIS           0x0C
#define MCP9600_ALERT2_HYSTERESIS           0x0D
#define MCP9600_ALERT3_HYSTERESIS           0x0E
#define MCP9600_ALERT4_HYSTERESIS           0x0F
#define MCP9600_ALERT1_LIMIT                0x10
#define MCP9600_ALERT2_LIMIT                0x11
#define MCP9600_ALERT3_LIMIT                0x12
#define MCP9600_ALERT4_LIMIT                0x13
#define MCP9600_DEVICE_ID                   0x20

// MCP9600 Constants
#define MCP9600_DEVICE_ID_VALUE 0x40
#define MCP9600_TEMP_RESOLUTION 0.0625f

typedef enum
{
    TYPE_K,
    TYPE_J,
    TYPE_T,
    TYPE_N,
    TYPE_S,
    TYPE_E,
    TYPE_B,
    TYPE_R
} thermocouple_type_t;

typedef enum
{
    FILTER_OFF = 0x00,
    FILTER_MINIMUM = 0x01,
    FILTER_2 = 0x02,
    FILTER_3 = 0x03,
    FILTER_MIDDLE = 0x04,
    FILTER_5 = 0x05,
    FILTER_6 = 0x06,
    FILTER_MAXIMUM = 0x07
} filter_coefficient_t;

typedef enum
{
    ALERT_1,
    ALERT_2,
    ALERT_3,
    ALERT_4
} alert_t;


class MCP9600
{
public:
    MCP9600(uint device_address, i2c_inst_t* i2c, uint alert1_pin, 
        uint alert2_pin, uint alert3_pin , uint alert4_pin);
    MCP9600(uint device_address, i2c_inst_t* i2c, thermocouple_type_t thermocouple_type = thermocouple_type_t::TYPE_K, 
            filter_coefficient_t coefficient = filter_coefficient_t::FILTER_MIDDLE, uint alert1_pin = -1, uint alert2_pin = -1, 
            uint alert3_pin = -1, uint alert4_pin = -1);
    void init();
    bool isConnected();

    float getHotJunctionTemperature();
    float getJunctionTemperatureDelta();
    float getColdJunctionTemperature();

    void setAlertConfig(alert_t alert);
    void setAlertLimit(alert_t alert, uint16_t limit);
    void setAlertHysteresis(alert_t alert, uint8_t hysteresis);
    void alertAttachInterrupt(alert_t alert, void (*callback)(uint gpio, uint32_t events));
    void alertInterruptClear(alert_t alert);

    float convertTemperature(uint8_t* data);
    uint16_t convertTemperature(float data);

    uint getId();
    uint getRevision();
private:
    bool is_connected;
    i2c_inst_t* i2c;
    uint device_address;
    thermocouple_type_t thermocouple_type;
    filter_coefficient_t coefficient;
    uint alert1_pin;
    uint alert2_pin;
    uint alert3_pin;
    uint alert4_pin;
    uint id;
    uint revision;
    uint8_t alertConfig[4] = { 0x0 };

    uint* getAlertPin(alert_t alert);
    uint getAlertConfigRegister(alert_t alert);
    uint getAlertLimitRegister(alert_t alert);
    uint getAlertHysteresisRegister(alert_t alert);

    bool verifyConnection();
    void writeRegister(uint8_t reg, uint8_t* data, uint8_t length);
    void readRegister(uint8_t reg, uint8_t* data, uint8_t length);
};