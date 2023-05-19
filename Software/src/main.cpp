#include "include/Defines.hpp"

// Configure the display
Display_Pins displayPins = {
#ifndef __INTELLISENSE__
	.rst = DISPLAY_RST,
	.dc = DISPLAY_DC,
	.cs = DISPLAY_CS,
	.sda = DISPLAY_MOSI,
	.scl = DISPLAY_SCK,
	.bl = DISPLAY_BL,
#endif
};

Display_Params displayParams = {
#ifndef __INTELLISENSE__
	.height = DISPLAY_HEIGHT,
	.width = DISPLAY_WIDTH,
	.columnOffset1 = DISPLAY_OFFSET_X0,
	.columnOffset2 = DISPLAY_OFFSET_X1,
	.rowOffset1 = DISPLAY_OFFSET_Y0,
	.rowOffset2 = DISPLAY_OFFSET_Y1,
	.rotation = DISPLAY_ROTATION,
#endif
};

// initialize the EEPROM and MCP9600 ICs
Memory memory(EEPROM_ADDRESS, i2c0);
MCP9600 mcp9600_1(MCP9600_1_ADDRESS, i2c0);
MCP9600 mcp9600_2(MCP9600_2_ADDRESS, i2c0);
MCP9600 mcp9600_3(MCP9600_3_ADDRESS, i2c0);
Display display(spi0, displayPins, displayParams, GC9A01);

bool led_on = false;
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

	// initialize the MCP9600 ICs
	mcp9600_1.init();
	mcp9600_2.init();
	mcp9600_3.init();

	// verify that the EEPROM is connected
	bool EEPROM_is_connected = memory.verifyConnection();
	//display.fill(Colors::White);

	//display.drawFilledRectangle(Point(50,50), Point(150,150), Colors::Red);
	display.drawLine(Point(0,0), Point(240,240));
	display.drawLine(Point(240,0), Point(0,240));
	display.drawCircle(Point(120,120), 120);
	//display.setCursor(Point(50,50));
	//display.print("Hi!");

	// enable watchdog requiring a reset every 1000ms
	watchdog_enable(1000, 1);

	// infinite loop
	while(1)
	{
		if(mcp9600_3.isConnected())
		{
			printf("\nHot: %.2f, Cold: %.2f, Delta: %.2f\n", 
				mcp9600_3.getHotJunctionTemperature(),
				mcp9600_3.getColdJunctionTemperature(),
				mcp9600_3.getJunctionTemperatureDelta());
		}

		//display.setCursor(Point(50,20));
		//display.print(mcp9600_3.getHotJunctionTemperature());

		// delay
		sleep_ms(500);
		gpio_put(LED_PIN, led_on);
		led_on = !led_on;

		watchdog_update();
	}
}