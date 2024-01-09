#pragma once

#include "pico/stdlib.h"
#include "PID.hpp"
#include "MCP9600.hpp"
#include "error.hpp"

#define MAXIMUM_ON_TIME 5000000 // Decides the PWM frequency, 1s is 1Hz, 0.5s is 2Hz, etc.

// Stores all possible configurations, where each bit represents a heater
typedef enum : int
{
    HEATER_1        = 0b0001,   // First bit set
    HEATER_2        = 0b0010,   // Second bit set
    HEATER_3        = 0b0100,   // Third bit set
    HEATER_1_2      = 0b0011,   // First and second bits set
    HEATER_1_3      = 0b0101,   // First and third bits set
    HEATER_2_3      = 0b0110,   // Second and third bits set
    HEATER_1_2_3    = 0b0111    // All bits set
} heaters_t;

class Oven
{
public:
    Oven(PID* pid, MCP9600* sensor1, MCP9600* sensor2, MCP9600* sensor3, int heaterPin1, int heaterPin2, int heaterPin3, int indicatorPin);
    
    error_state_t init(heaters_t heaters = heaters_t::HEATER_1_2_3);
    float getReading();
    void setHeaterConfiguration(heaters_t heaters);

    error_state_t updateHeaters(bool active, float target);
    
private:
    PID* pid, *pid2, *pid3;
    MCP9600* sensor1, *sensor2, *sensor3;
    bool sensor1Active = false, sensor2Active = false, sensor3Active = false;
    int sensorCount = 0;
    int heaterPin1, heaterPin2, heaterPin3, indicatorPin;
    int heaterConfiguration;
    float temperature = 0.0f;

    float reading();
    bool getPinState(int pidOutput);
    void setHeaterPins(bool state);
};