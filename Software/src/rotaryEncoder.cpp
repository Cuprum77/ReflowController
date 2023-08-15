#include "rotaryEncoder.hpp"

/**
 * @brief Construct a new Rotary Encoder object
 * @param pinA The pin connected to the A output of the rotary encoder
 * @param pinB The pin connected to the B output of the rotary encoder
 * @param buttonPin The pin connected to the button output of the rotary encoder
 * @param upperLimit The upper limit of the counter, default is CRITICAL_TEMPERATURE. If we reach this value, we overflow to 0
*/
RotaryEncoder::RotaryEncoder(int pinA, int pinB, int buttonPin, int upperLimit)
{

}

/**
 * @brief Initialize the rotary encoder object
*/
void RotaryEncoder::init()
{

}

/**
 * @brief Update the rotary encoder object
 * @note This function should be called in the main loop
*/
void RotaryEncoder::update()
{

}

/**
 * @brief Get the button state
 * @return true if the button is pressed, false otherwise
*/
bool RotaryEncoder::getButton()
{
    return false;
}

/**
 * @brief Reset the encoder value
*/
void RotaryEncoder::resetEncoderValue()
{
    this->encoderValue = 0;
}

/**
 * @brief Get the encoder position
 * @return The value of the encoder
*/
int RotaryEncoder::getEncoderValue()
{
    return this->encoderValue;
}