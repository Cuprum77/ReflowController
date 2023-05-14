// This is a header guard. It prevents the file from being included more than once.
#pragma once

/***
 *      ____  _           
 *     |  _ \(_)_ __  ___ 
 *     | |_) | | '_ \/ __|
 *     |  __/| | | | \__ \
 *     |_|   |_|_| |_|___/
 *                        
 */

// Alarm pins
#define ALARM_PIN_1_A 5
#define ALARM_PIN_2_A 4
#define ALARM_PIN_3_A 3
#define ALARM_PIN_4_A 2

#define ALARM_PIN_1_B 13
#define ALARM_PIN_2_B 12
#define ALARM_PIN_3_B 11
#define ALARM_PIN_4_B 10

#define ALARM_PIN_1_C 9
#define ALARM_PIN_2_C 8
#define ALARM_PIN_3_C 7
#define ALARM_PIN_4_C 6

// I2C pins
//#define I2C_SDA 0
//#define I2C_SCL 1

// dev
#define I2C_SDA 20
#define I2C_SCL 21

// Button pins
#define BUTTON_PIN 14
#define BUTTON_LED_PIN 15

// Display pins
#define DISPLAY_DC 16
#define DISPLAY_CS 17
#define DISPLAY_SCK 18
#define DISPLAY_MOSI 19
#define DISPLAY_RST 20

// Encouder pins
#define ENCODER_A 23
#define ENCODER_B 22
#define ENCODER_SW 21

// Built in LED
#define LED_PIN 25

// Output Enable
#define OUTPUT_ENABLE_1_PIN 26
#define OUTPUT_ENABLE_2_PIN 27
#define OUTPUT_ENABLE_3_PIN 28


/***
 *     __     __         _       _     _           
 *     \ \   / /_ _ _ __(_) __ _| |__ | | ___  ___ 
 *      \ \ / / _` | '__| |/ _` | '_ \| |/ _ \/ __|
 *       \ V / (_| | |  | | (_| | |_) | |  __/\__ \
 *        \_/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
 *                                                 
 */

// I2C Speed
#define I2C_SPEED 100000


/***
 *         _       _     _                             
 *        / \   __| | __| |_ __ ___  ___ ___  ___  ___ 
 *       / _ \ / _` |/ _` | '__/ _ \/ __/ __|/ _ \/ __|
 *      / ___ \ (_| | (_| | | |  __/\__ \__ \  __/\__ \
 *     /_/   \_\__,_|\__,_|_|  \___||___/___/\___||___/
 *                                                     
 */

// MCP9600 Breakout boards
#define MCP9600_1_ADDRESS 0x60
#define MCP9600_2_ADDRESS 0x61  
#define MCP9600_3_ADDRESS 0x67

// EEPROM IC
#define EEPROM_ADDRESS 0x50


/***
 *      ___            _           _           
 *     |_ _|_ __   ___| |_   _  __| | ___  ___ 
 *      | || '_ \ / __| | | | |/ _` |/ _ \/ __|
 *      | || | | | (__| | |_| | (_| |  __/\__ \
 *     |___|_| |_|\___|_|\__,_|\__,_|\___||___/
 *                                             
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "tusb.h"

#include "Memory.hpp"
#include "MCP9600.hpp"
#include "version.h"



/***
 *      ____  _         _____           _    ____        _        
 *     |  _ \(_) ___ __|_   _|__   ___ | |  |  _ \  __ _| |_ __ _ 
 *     | |_) | |/ __/ _ \| |/ _ \ / _ \| |  | | | |/ _` | __/ _` |
 *     |  __/| | (_| (_) | | (_) | (_) | |  | |_| | (_| | || (_| |
 *     |_|   |_|\___\___/|_|\___/ \___/|_|  |____/ \__,_|\__\__,_|
 *                                                               
 */

bi_decl(bi_program_name("Reflow Oven Controller"));
bi_decl(bi_program_description("Pico based PID controller for a reflow oven with temperature monitoring and control."));
bi_decl(bi_program_version_string(GIT_COMMIT_HASH));
bi_decl(bi_program_url("https://github.com/Cuprum77/ReflowController"));
