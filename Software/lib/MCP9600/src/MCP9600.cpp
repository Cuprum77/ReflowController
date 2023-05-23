#include "MCP9600.hpp"


/**
 * @brief Construct a new MCP9600::MCP9600 object
 * @param device_address The address of the MCP9600 IC
 * @param i2c The I2C bus to use
 * @param thermocouple_type The type of thermocouple used
 * @param filter_coefficient The filter coefficient to use
 */
MCP9600::MCP9600(uint device_address, i2c_inst_t* i2c, 
        thermocouple_type_t thermocouple_type, 
        filter_coefficient_t coefficient)
{
    this->i2c = i2c;
    this->device_address = device_address;
    this->thermocouple_type = thermocouple_type;
    this->coefficient = coefficient;
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
double MCP9600::getTemperature()
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
double MCP9600::getDifference()
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
double MCP9600::getAmbientTemperature()
{
    // create a buffer to store the data
    uint8_t buffer[2];
    // read the data from the register
    this->readRegister(MCP9600_COLD_JUNCTION_TEMP, buffer, 2);
    // return the temperature
    return this->convertTemperature(buffer);
}

/**
 * @brief Convert a float temperature to a uint16_t
 * @param temperature The temperature to convert
 * @return The converted temperature
 */
uint16_t MCP9600::convertTemperature(double temperature)
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
 * @brief Convert a raw temperature to degrees celsius
 * @param raw The raw temperature
 * @return The temperature in degrees celsius
 * @note Expects MSB first
*/
double MCP9600::convertTemperature(uint8_t* data)
{
    // convert the data to a float
    float temperature = (float)((data[0] << 0x8) | data[1]);
    // multiply by the resolution to get the temperature in degrees celsius
    temperature *= MCP9600_TEMP_RESOLUTION;
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