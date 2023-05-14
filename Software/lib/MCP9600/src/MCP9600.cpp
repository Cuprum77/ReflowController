#include "MCP9600.hpp"


/**
 * @brief Construct a new MCP9600::MCP9600 object
 * @param device_address The address of the MCP9600 IC
 * @param i2c The I2C bus to use
 * @param alert1_pin The pin to use for alert 1
 * @param alert2_pin The pin to use for alert 2
 * @param alert3_pin The pin to use for alert 3
 * @param alert4_pin The pin to use for alert 4
 */
MCP9600::MCP9600(uint device_address, i2c_inst_t* i2c, 
    uint alert1_pin, uint alert2_pin, uint alert3_pin, uint alert4_pin)
{
    this->i2c = i2c;
    this->device_address = device_address;
    this->thermocouple_type = thermocouple_type_t::TYPE_K;
    this->coefficient = filter_coefficient_t::FILTER_MIDDLE;
    this->alert1_pin = alert1_pin;
    this->alert2_pin = alert2_pin;
    this->alert3_pin = alert3_pin;
    this->alert4_pin = alert4_pin;
}

/**
 * @brief Construct a new MCP9600::MCP9600 object
 * @param device_address The address of the MCP9600 IC
 * @param i2c The I2C bus to use
 * @param thermocouple_type The type of thermocouple used
 * @param filter_coefficient The filter coefficient to use
 * @param alert1_pin The pin to use for alert 1
 * @param alert2_pin The pin to use for alert 2
 * @param alert3_pin The pin to use for alert 3
 * @param alert4_pin The pin to use for alert 4
 */
MCP9600::MCP9600(uint device_address, i2c_inst_t* i2c, thermocouple_type_t thermocouple_type, 
            filter_coefficient_t coefficient, uint alert1_pin, uint alert2_pin, uint alert3_pin, uint alert4_pin)
{
    this->i2c = i2c;
    this->device_address = device_address;
    this->thermocouple_type = thermocouple_type;
    this->coefficient = coefficient;
    this->alert1_pin = alert1_pin;
    this->alert2_pin = alert2_pin;
    this->alert3_pin = alert3_pin;
    this->alert4_pin = alert4_pin;
}

/**
 * @brief Initialize the MCP9600
 */
void MCP9600::init()
{
    // verify that the device is connected
    this->is_connected = this->verifyConnection();

    // if not connected, return
    if (!this->is_connected)
        return;

    // create a buffer to store the data
    uint8_t buffer[2] = {0};
    // read the data from the register
    this->readRegister(MCP9600_DEVICE_ID, buffer, 2);
    this->id = buffer[0];
    this->revision = buffer[1];

    // set the configuration register
    uint8_t config = 0x0000;
    config |= (uint8_t)this->thermocouple_type << 0x4 & 0x3;   // set the thermocouple type
    config |= (uint8_t)this->coefficient & 0x7;        // set the filter coefficient
    this->writeRegister(MCP9600_THERMOCOUPLE_SENSOR_CONFIG, &config, 1);
}

/**
 * @brief Return whether or not the MCP9600 is connected
 * @return True if connected, false otherwise
 */
bool MCP9600::isConnected()
{
    return this->is_connected;
}

/**
 * @brief Get the hot junction temperature
 * @return The hot junction temperature in degrees celsius
 */
float MCP9600::getHotJunctionTemperature()
{
    // create a buffer to store the data
    uint8_t buffer[2];
    // read the data from the register
    this->readRegister(MCP9600_HOT_JUNCTION_TEMP, buffer, 2);
    // return the temperature
    return this->convertTemperature(buffer);
}

/**
 * @brief Get the junction temperature delta
 * @return The junction temperature delta in degrees celsius
 */
float MCP9600::getJunctionTemperatureDelta()
{
    // create a buffer to store the data
    uint8_t buffer[2];
    // read the data from the register
    this->readRegister(MCP9600_DELTA_TEMP, buffer, 2);
    // return the temperature
    return this->convertTemperature(buffer);
}

/**
 * @brief Get the cold junction temperature
 * @return The cold junction temperature in degrees celsius
 */
float MCP9600::getColdJunctionTemperature()
{
    // create a buffer to store the data
    uint8_t buffer[2];
    // read the data from the register
    this->readRegister(MCP9600_COLD_JUNCTION_TEMP, buffer, 2);
    // return the temperature
    return this->convertTemperature(buffer);
}

/**
 * @brief Set the alert configuration for the given alert
 * @param alert The alert to set the configuration for
*/
void MCP9600::setAlertConfig(alert_t alert)
{
    // store the configuration in the local array
    this->alertConfig[alert] = 0x1;

    // write the data to the register
    this->writeRegister(this->getAlertConfigRegister(alert), &this->alertConfig[alert], 1);
}

/**
 * @brief Set the alert temperature limit
 * @param alert The alert to set the limit for
 * @param limit The limit to set
 */
void MCP9600::setAlertLimit(alert_t alert, uint16_t limit)
{
    // create a buffer to store the data
    uint8_t buffer[2] = {
        (uint8_t)(limit >> 8),
        (uint8_t)(limit & 0xFF)
    };

    // write the data to the register
    this->writeRegister(this->getAlertLimitRegister(alert), buffer, 2);
}

/**
 * @brief Set the alert hysterisis
 * @param alert The alert to set the hysterisis for
 * @param hysterisis The hysterisis to set
 * @note The number should directly correspond to the temperature in degrees celsius
 */
void MCP9600::setAlertHysteresis(alert_t alert, uint8_t hysterisis)
{
    // write the data to the register
    this->writeRegister(this->getAlertHysteresisRegister(alert), &hysterisis, 1);
}

/**
 * @brief Attach a callback to the alert 1 pin
 * @param callback The callback to attach
 * @note The callback should take the form of void callback(uint gpio, uint32_t events)
 */
void MCP9600::alertAttachInterrupt(alert_t alert, void (*callback)(uint gpio, uint32_t events))
{
    // get the relevant gpio
    uint* pin = this->getAlertPin(alert);
    // verify that pin is not null or -1
    if (pin == nullptr || *pin == -1) return;

    // infer the rise/fall from the alert config
    bool rising = (this->alertConfig[alert] >> 2) & 0x1;
    // convert to event mask
    uint32_t event = rising ? GPIO_IRQ_EDGE_RISE : GPIO_IRQ_EDGE_FALL;

    // attach the callback
    gpio_set_irq_enabled_with_callback(*pin, event, true, callback);
}

/**
 * @brief Clear the alert interrupt
 * @param alert The alert to clear the interrupt for
 */
void MCP9600::alertInterruptClear(alert_t alert)
{
    // get the relevant gpio
    uint* pin = this->getAlertPin(alert);
    // verify that pin is not null or -1
    if (pin == nullptr || *pin == -1) return;

    // clear the interrupt
    gpio_acknowledge_irq(*pin, GPIO_IRQ_EDGE_RISE);

    // clear the interrupt on the device
    uint8_t config = this->alertConfig[alert] | 0x1 << 7;
    this->writeRegister(this->getAlertConfigRegister(alert), &config, 1);
}

/**
 * @brief Convert a float temperature to a uint16_t
 * @param temperature The temperature to convert
 * @return The converted temperature
 */
uint16_t MCP9600::convertTemperature(float temperature)
{
    // check if data is negative
    bool is_negative = temperature < 0;
    // convert the temperature to a uint16_t
    uint16_t data = (uint16_t)(temperature / MCP9600_TEMP_RESOLUTION);
    // if the data is negative, set the sign bit
    if (is_negative) data |= 0x1 << 15;
    // return the data
    return data;
}

/**
 * @brief Get the device id>
 * @return The device id
 */
uint MCP9600::getId()
{
    return this->id;
}

/**
 * @brief Get the device revision
 * @return The device revision
 */
uint MCP9600::getRevision()
{
    return this->revision;
}



/**
 * @private
 * @brief Get the alert pin for a given alert
 * @param alert The alert to get the pin for
 * @return The alert pin
*/
uint* MCP9600::getAlertPin(alert_t alert)
{
    switch (alert)
    {
    case ALERT_1:
        return &this->alert1_pin;
    case ALERT_2:
        return &this->alert2_pin;
    case ALERT_3:
        return &this->alert3_pin;
    case ALERT_4:
        return &this->alert4_pin;
    default:
        return NULL;
    }
}

/**
 * @private
 * @brief Get the alert configuration register for a given alert
 * @param alert The alert to get the register for
 * @return The alert configuration register
*/
uint MCP9600::getAlertConfigRegister(alert_t alert)
{
    switch (alert)
    {
    case ALERT_1:
        return MCP9600_ALERT1_CONFIG;
    case ALERT_2:
        return MCP9600_ALERT2_CONFIG;
    case ALERT_3:
        return MCP9600_ALERT3_CONFIG;
    case ALERT_4:
        return MCP9600_ALERT4_CONFIG;
    default:
        return NULL;
    }
}

/**
 * @private
 * @brief Get the alert limit register for a given alert
 * @param alert The alert to get the register for
 * @return The alert limit register
*/
uint MCP9600::getAlertLimitRegister(alert_t alert)
{
    switch (alert)
    {
    case ALERT_1:
        return MCP9600_ALERT1_LIMIT;
    case ALERT_2:
        return MCP9600_ALERT2_LIMIT;
    case ALERT_3:
        return MCP9600_ALERT3_LIMIT;
    case ALERT_4:
        return MCP9600_ALERT4_LIMIT;
    default:
        return NULL;
    }
}

/**
 * @private
 * @brief Get the alert hyteresis register for a given alert
 * @param alert The alert to get the register for
 * @return The alert hysteresis register
*/
uint MCP9600::getAlertHysteresisRegister(alert_t alert)
{
    switch (alert)
    {
    case ALERT_1:
        return MCP9600_ALERT1_HYSTERESIS;
    case ALERT_2:
        return MCP9600_ALERT2_HYSTERESIS;
    case ALERT_3:
        return MCP9600_ALERT3_HYSTERESIS;
    case ALERT_4:
        return MCP9600_ALERT4_HYSTERESIS;
    default:
        return NULL;
    }
}


/**
 * @private
 * @brief Convert a raw temperature to degrees celsius
 * @param raw The raw temperature
 * @return The temperature in degrees celsius
 * @note Expects MSB first
*/
float MCP9600::convertTemperature(uint8_t* data)
{
    // convert the data to a float
    float temperature = (float)((data[0] << 0x8) | data[1]);
    // multiply by the resolution to get the temperature in degrees celsius
    temperature *= MCP9600_TEMP_RESOLUTION;
    // if negative temperature, subtract 4096
    if (data[0] & 0x8) temperature -= 4096;
    return temperature;
}

/**
 * @private
 * @brief Verify that the MCP9600 is connected
 * @return True if connected, false if not
 */
bool MCP9600::verifyConnection()
{
    // check if we get a response from the eeprom by dummy writing to it
    unsigned char data;
    // if the dummy write fails, it should return a negative number
    int ret = i2c_read_blocking(this->i2c, this->device_address, &data, 1, false);
    return !(ret < 0);
}

/**
 * @private
 * @brief Write a byte to the MCP9600
 * @param target_address The address to write to
 * @param data The data to write as a pointer
 * @param length The length of the data to write
 */
void MCP9600::writeRegister(uint8_t target_address, uint8_t *data, uint8_t length)
{
    i2c_write_blocking(this->i2c, this->device_address, &target_address, 1, false);
    i2c_write_blocking(this->i2c, this->device_address, data, length, false);
}

/**
 * @private
 * @brief Read a byte from the MCP9600
 * @param target_address The address to read from
 * @param data The data to read as a pointer
 * @param length The length of the data to read
 */
void MCP9600::readRegister(uint8_t target_address, uint8_t *data, uint8_t length)
{
    int bytes_read = 0;
    i2c_write_blocking(this->i2c, this->device_address, &target_address, 1, true);
    bytes_read += i2c_read_blocking(this->i2c, this->device_address, data, length, false);

    // if the number of bytes read is not equal to the length, then there was an error
    // we will simply fill the the buffer with 0s to clear the buffer
    if (bytes_read != length)
    {
        for(int i = 0; i < length; i++)
            data[i] = 0;
    }
}