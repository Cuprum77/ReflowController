#include "PID.hpp"

/**
 * @brief Construct a new PID::PID object
*/
PID::PID()
{
    this->controller.Kp = 0;
    this->controller.Ki = 0;
    this->controller.Kd = 0;
    this->controller.tau = 0.02f;
    this->controller.min = -10;
    this->controller.max = 10;
    this->controller.sampleTime = 0.01f;
}

/**
 * @brief Construct a new PID::PID object
 * @param kp The proportional gain
 * @param ki The integral gain
 * @param kd The derivative gain
 */
PID::PID(float kp, float ki, float kd)
{
    this->controller.Kp = kp;
    this->controller.Ki = ki;
    this->controller.Kd = kd;
    this->controller.tau = 0.02f;
    this->controller.min = -10;
    this->controller.max = 10;
    this->controller.sampleTime = 0.01f;
}

/**
 * @brief Construct a new PID::PID object
 * @param kp The proportional gain
 * @param ki The integral gain
 * @param kd The derivative gain
 * @param tau The derivative low-pass filter time constant
 * @param min The minimum output value
 * @param max The maximum output value
 * @param sampleTime The sample time
 */
PID::PID(float kp, float ki, float kd, float tau, float min, float max, float sampleTime)
{
    this->controller.Kp = kp;
    this->controller.Ki = ki;
    this->controller.Kd = kd;
    this->controller.tau = tau;
    this->controller.min = min;
    this->controller.max = max;
    this->controller.sampleTime = sampleTime;
}

/**
 * @brief Update the PID controller
 * @param setPoint The set point
 * @param measurement The measurement
 */
void PID::update(float setPoint, float measurement)
{
    // Calculate error
    float error = setPoint - measurement;

    // Calculate the proportional term
    float proportional = this->controller.Kp * error;

    // Calculate the integral term
    this->controller.integrator = 
        this->controller.integrator + 0.5f * this->controller.Ki 
        * this->controller.sampleTime * (error + this->controller.lastError);

    // Clamp the integrator
    if (this->controller.integrator > this->controller.max)
    {
        this->controller.integrator = this->controller.max;
    }
    else if (this->controller.integrator < this->controller.min)
    {
        this->controller.integrator = this->controller.min;
    }

    // Calculate the derivative term
    this->controller.differentiator = 
        -(2.0f * this->controller.Kd * (measurement - this->controller.lastMeasurement)
        + (2.0f * this->controller.tau - this->controller.sampleTime) * this->controller.differentiator)
        / (2.0f * this->controller.tau + this->controller.sampleTime);

    // Calculate the output
    this->controller.output = proportional + this->controller.integrator + this->controller.differentiator;

    // Clamp the output
    if (this->controller.output > this->controller.max)
    {
        this->controller.output = this->controller.max;
    }
    else if (this->controller.output < this->controller.min)
    {
        this->controller.output = this->controller.min;
    }

    // Store the last measurement
    this->controller.lastMeasurement = measurement;
    this->controller.lastError = error;
}

/**
 * @brief Reset the PID controller
 */
void PID::reset()
{
    this->controller.integrator = 0;
    this->controller.differentiator = 0;
    this->controller.lastMeasurement = 0;
    this->controller.output = 0;
}

/**
 * @brief Set the proportional gain
 * @param kp The proportional gain
 */
void PID::setKp(float kp)
{
    this->controller.Kp = kp;
}

/**
 * @brief Set the integral gain
 * @param ki The integral gain
 */
void PID::setKi(float ki)
{
    this->controller.Ki = ki;
}

/**
 * @brief Set the derivative gain
 * @param kd The derivative gain
 */
void PID::setKd(float kd)
{
    this->controller.Kd = kd;
}

/**
 * @brief Set the derivative low-pass filter time constant
 * @param tau The derivative low-pass filter time constant
 */
void PID::setTau(float tau)
{
    this->controller.tau = tau;
}

/**
 * @brief Set the minimum output value
 * @param min The minimum output value
 */
void PID::setMin(float min)
{
    this->controller.min = min;
}

/**
 * @brief Set the maximum output value
 * @param max The maximum output value
 */
void PID::setMax(float max)
{
    this->controller.max = max;
}

/**
 * @brief Set the sample time
 * @param sampleTime The sample time
 */
void PID::setSampleTime(float sampleTime)
{
    this->controller.sampleTime = sampleTime;
}

/**
 * @brief Get the output
 * @return bool The output
 */
bool PID::getOutput()
{
    if (this->controller.output > 0)
        return true;
    else
        return false;
}

/**
 * @brief Get the output
 * @return float The output
 */
float PID::output()
{
    return this->controller.output;
}