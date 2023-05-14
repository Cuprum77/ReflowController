#include "include/Defines.hpp"

// initialize the EEPROM and MCP9600 ICs
Memory memory = Memory(EEPROM_ADDRESS, i2c0);
MCP9600 mcp9600_1 = MCP9600(MCP9600_1_ADDRESS, i2c0);
MCP9600 mcp9600_2 = MCP9600(MCP9600_2_ADDRESS, i2c0);
MCP9600 mcp9600_3 = MCP9600(MCP9600_3_ADDRESS, i2c0);

int main()
{
	// initialize the stdio
	stdio_init_all();

	// enable the led
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	// initialize the I2C0 bus
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);
	i2c_init(i2c0, I2C_SPEED);
	// set the binary data to show the pins used for I2C0
	bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));
	
	// enable watchdog requiring a reset every 1000ms
	watchdog_enable(1000, 1);

	// initialize the MCP9600 ICs
	mcp9600_1.init();
	mcp9600_2.init();
	mcp9600_3.init();

	// verify that the EEPROM is connected
	bool EEPROM_is_connected = memory.verifyConnection();

	// buffer for the USB serial
	char buffer[64];

	// infinite loop
	while(1)
	{
		// blink the LED
		gpio_put(LED_PIN, 1);
		sleep_ms(300);
		gpio_put(LED_PIN, 0);
		sleep_ms(300);

		if(mcp9600_3.isConnected())
		{
			printf("\nDevice ID: 0x%x, Revision ID: 0x%x\n", mcp9600_3.getId(), mcp9600_3.getRevision());
			printf("Hot Junction Temperature: %.4f\n", mcp9600_3.getHotJunctionTemperature());
			printf("Junction Temperature Delta: %.4f\n", mcp9600_3.getJunctionTemperatureDelta());
			printf("Cold Junction Temperature: %.4f\n", mcp9600_3.getColdJunctionTemperature());
			printf("Git Hash: %s\n", GIT_COMMIT_HASH);
			printf("Git Branch: %s\n", GIT_BRANCH);
		}

		watchdog_update();
	}
}