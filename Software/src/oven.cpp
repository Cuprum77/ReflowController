#include "oven.hpp"

/**
 * @brief Constructor for the oven class
 * @param pid The PID controller to use
 * @param sensor1 The first sensor to use
 * @param sensor2 The second sensor to use
 * @param sensor3 The third sensor to use
 * @param heaterPin1 The pin for the first heater
 * @param heaterPin2 The pin for the second heater
 * @param heaterPin3 The pin for the third heater
 * @param indicatorPin The pin for the indicator led
*/
Oven::Oven(PID* pid, MCP9600* sensor1, MCP9600* sensor2, MCP9600* sensor3, int heaterPin1, int heaterPin2, int heaterPin3, int indicatorPin)
{
    this->pid = pid;
    this->sensor1 = sensor1;
    this->sensor2 = sensor2;
    this->sensor3 = sensor3;
    this->heaterPin1 = heaterPin1;
    this->heaterPin2 = heaterPin2;
    this->heaterPin3 = heaterPin3;
    this->indicatorPin = indicatorPin;
}

/**
 * @brief Initialize the oven
 * @param heaters The heaters to use, default is all
 * @return The error state of the function
*/
error_state_t Oven::init(heaters_t heaters)
{
    // Initialize the error return variable
    int error = 0;

    // Set the heater configuration
    this->heaterConfiguration = (unsigned int)heaters;

    // Initialize the sensors
    this->sensor1->init();
    this->sensor2->init();
    this->sensor3->init();

    // Check if the sensors are present
    this->sensor1Active = this->sensor1->isConnected();
    this->sensor2Active = this->sensor2->isConnected();
    this->sensor3Active = this->sensor3->isConnected();

    // Store the sensor count
    this->sensorCount = this->sensor1Active + this->sensor2Active + this->sensor3Active;

    // Modify the error variable based on the sensor status
    error |= (this->sensor1Active) ? (int)error_state_t::NO_ERROR : (int)error_state_t::SENSOR_SENSOR1_ERROR;
    error |= (this->sensor2Active) ? (int)error_state_t::NO_ERROR : (int)error_state_t::SENSOR_SENSOR2_ERROR;
    error |= (this->sensor3Active) ? (int)error_state_t::NO_ERROR : (int)error_state_t::SENSOR_SENSOR3_ERROR;

    // Initialize the heater pins
    gpio_init(this->heaterPin1);
    gpio_init(this->heaterPin2);
    gpio_init(this->heaterPin3);
    gpio_init(this->indicatorPin);

    // Set the heater pins to output
    gpio_set_dir(this->heaterPin1, GPIO_OUT);
    gpio_set_dir(this->heaterPin2, GPIO_OUT);
    gpio_set_dir(this->heaterPin3, GPIO_OUT);
    gpio_set_dir(this->indicatorPin, GPIO_OUT);

    return (error_state_t)error;
}

/**
 * @brief Access variable to get the current temperature
 * @return Returns the average of the three sensors, or MAX_INT if no sensors are active
 * @note Will not average sensors which are not present!
*/
int Oven::reading()
{
    // If no sensors are active, return 0x7fffffff
    if(!this->sensorCount) return 0x7fffffff;

    // Declare a variable to hold the sum
    int sum = 0;

    // Get the temperature from each sensor if it is active
    if(this->sensor1Active) sum += this->sensor1->getTemperature();
    if(this->sensor2Active) sum += this->sensor2->getTemperature();
    if(this->sensor3Active) sum += this->sensor3->getTemperature();

    return sum / this->sensorCount;
}

/**
 * @brief Set the heater configuration for the oven
 * @param heaters The heaters to use
*/
void Oven::setHeaterConfiguration(heaters_t heaters)
{
    this->heaterConfiguration = (unsigned int)heaters;
}

/**
 * @brief Update the heaters
 * @param active Whether the oven is active or not
 * @param target The target temperature
 * @return The error state of the function
 * @note This should be called in the main loop
*/
error_state_t Oven::updateHeaters(bool active, int target)
{
    // Initialize the error return variable
    int error = 0;

    // Check if the oven is not active
    if (!active)
    {
        // Turn off the heaters
        setHeaterPins(false);

        // Turn off the indicator led
        gpio_put(this->indicatorPin, false);
    }
    else
    {
        // Get the latest temperature measurement
        int measurement = reading();

        // If temperature is MAX_INT, there was an error
        if(measurement == 0x7fffffff)
        {
            error |= (int)error_state_t::SENSOR_ALL_ERROR;
            measurement = 0;
        }

        // Update the PID
        this->pid->update(target, measurement);

        // Get the PID output
        int pidOutput = this->pid->getOutput();
        // Apply the PID output to the heater
        bool state = getPinState(pidOutput);
        // Set the corresponding heater pins
        setHeaterPins(state);
        // Set the indicator led
        gpio_put(this->indicatorPin, state);
    }

    return (error_state_t)error;
}

/**
 * @private
 * @brief Convert the PID output to a bool based on time
 * @param pidOutput The PID output
 * @return Whether the pin should be on or off
*/
bool Oven::getPinState(int pidOutput)
{
    // Convert the pid output to a 0-100% value
    unsigned long _pidOutput = (_pidOutput > 0) ? ((_pidOutput * 1000000) / 127) : 0;

    // Set the on and off times
    unsigned long onTime = _pidOutput;
    unsigned long offTime = 1000000 - _pidOutput;

    // Figure out if the pin should be on or off
    bool state = ((time_us_64() % (onTime + offTime)) < onTime);
    return state;
}

/**
 * @private
 * @brief Helper function to set the heater pins depending on the heater configuration
 * @param state Whether the pins should be on or off
*/
void Oven::setHeaterPins(bool state)
{
    // Set the first heater if it is active, else set it to off
    if(this->heaterConfiguration & 0b0001) 
        gpio_put(this->heaterPin1, state);
    else
        gpio_put(this->heaterPin1, false);

    // Do the same for heater 2
    if(this->heaterConfiguration & 0b0010) 
        gpio_put(this->heaterPin2, state);
    else
        gpio_put(this->heaterPin2, false);

    // And for heater 3
    if(this->heaterConfiguration & 0b0100) 
        gpio_put(this->heaterPin3, state);
    else
        gpio_put(this->heaterPin3, false);
}