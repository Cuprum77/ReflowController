#pragma once

// Error state enum
typedef enum : int
{
    // OK state (No bits set)
    NO_ERROR                = 0x0000,               // 00000000 00000000 00000000 00000000
    // Heater errors (4 bits, 0x00001 - 0x00008)
    HEATER_ERROR            = 0x0001,               // 00000000 00000000 00000000 00000001
    // Sensor errors (4 bits, 0x00010 - 0x00080)
    SENSOR_GENERAL_ERROR    = 0x0010,               // 00000000 00000000 00000000 00010000
    SENSOR_SENSOR1_ERROR    = 0x0030,               // 00000000 00000000 00000000 00110000
    SENSOR_SENSOR2_ERROR    = 0x0050,               // 00000000 00000000 00000000 01010000
    SENSOR_SENSOR3_ERROR    = 0x0090,               // 00000000 00000000 00000000 10010000
    SENSOR_ALL_ERROR        = 0x00F0,               // 00000000 00000000 00000000 11110000
    // EEPROM errors (4 bits, 0x00100 - 0x00800)
    EEPROM_ERROR            = 0x0100,               // 00000000 00000000 00000001 00000000
    EEPROM_READ_ERROR       = 0x0300,               // 00000000 00000000 00000011 00000000
    EEPROM_WRITE_ERROR      = 0x0500,               // 00000000 00000000 00000101 00000000
    EEPROM_CONNECTION_ERROR = 0x0900,               // 00000000 00000000 00001001 00000000
    // Encoder errors (4 bits, 0x01000 - 0x08000)
    ENCODER_ERROR           = 0x1000,               // 00000000 00000000 00010000 00000000
    // Display errors (4 bits, 0x10000 - 0x80000)
    DISPLAY_ERROR           = 0x10000,              // 00000000 00000001 00000000 00000000
    DISPLAY_NO_GAUGES_ERROR = 0x30000,              // 00000000 00000011 00000000 00000000
} error_state_t;