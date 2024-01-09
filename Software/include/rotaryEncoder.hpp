#pragma once

#include "pico/stdlib.h"

class RotaryEncoder 
{
public:
    RotaryEncoder(int pinA, int pinB, int buttonPin, int upperLimit = 255);

    void init();
    void update();

    bool getButton();
    void resetEncoderValue();
    int getEncoderValue();
private:
    int pinA, pinB, buttonPin, upperLimit;
    int encoderValue = 0;
};