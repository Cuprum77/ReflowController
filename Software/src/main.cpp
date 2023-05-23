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
PID pid(1.25f, 0.0f, 0.0f, 0.02f, -127.0f, 127.0f, 0.1f);

void runOven();

unsigned long offTime = 0;
unsigned long onTime = 0;
unsigned long lastTriggerTime = 0;
unsigned long runOccasionallyLastTime = 0;
bool isOn = false;
bool enabled = false;
int main()
{
	// initialize the stdio
	stdio_init_all();

	// enable the led
	gpio_init(LED_PIN);
	gpio_init(OUTPUT_ENABLE_1_PIN);
	gpio_init(BUTTON_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_set_dir(OUTPUT_ENABLE_1_PIN, GPIO_OUT);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);

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
	display.fill(Colors::RaspberryRed);
	//display.fillGradient(Colors::RaspberryRed, Colors::Derg, {0,0}, {DISPLAY_WIDTH, DISPLAY_HEIGHT});

	// enable watchdog requiring a reset every 1000ms
	watchdog_enable(1000, 1);

	// infinite loop
	uint itteration = 5;
	
	while(1)
	{
		// software pwm at 1 Hz
		if(enabled)
		{
			// turn on the relay if the time is right
			if((time_us_64() % (onTime + offTime) < onTime))
				isOn = true;
			else
				isOn = false;
		}
		else
		{
			isOn = false;
			gpio_put(OUTPUT_ENABLE_1_PIN, 0);
			lastTriggerTime = time_us_32();
		}

		// sleep for 10ms to avoid damaging the relay through PID seizures
		if((time_us_32() - runOccasionallyLastTime) >= 10000)
		{
			runOccasionallyLastTime = time_us_32();
			runOven();
		}

		watchdog_update();
	}
}

float prevTemp1 = 0;
float prevTemp2 = 0;
unsigned long lastUpdate = 0;
bool prevButton = true;
void runOven()
{
	double measurement = mcp9600_3.getTemperature();
	double target = 50.0f;
	double tempTarget = 50.0f;
	int length1 = 0;

	bool output = pid.getOutput();

	// read the button state and set the output enable pin
	bool button = gpio_get(BUTTON_PIN);
	// single shot it to avoid bouncing
	if(button && !prevButton)
		enabled = !enabled;
	prevButton = button;
	gpio_put(LED_PIN, enabled);

	if(enabled)
	{
		pid.update(tempTarget, measurement);
		target = 50.0f;
		char _pidOutput = (char)pid.output();
		unsigned long pidOutput = (_pidOutput > 0) ? ((_pidOutput * 1000000) / 127) : 0;;
		// set the on and off times
		onTime = pidOutput;
		offTime = 1000000 - pidOutput;
		gpio_put(OUTPUT_ENABLE_1_PIN, isOn);
	}
	else
	{
		pid.reset();
		target = mcp9600_3.getAmbientTemperature();
	}

	if(measurement >= 100)
	{
		int length1 = (display.getStringLength(measurement, 0, 2) + FONT_WIDTH * 2);
		display.setCursor(Point(
			(DISPLAY_WIDTH - length1) >> 1, 
			(DISPLAY_HEIGHT >> 1) - FONT_HEIGHT)
		);
		display.print(measurement, Colors::White, 0, 2);
		display.print("C", Colors::White, 2);
	}
	else
	{
		int length1 = (display.getStringLength(measurement, 1, 2) + FONT_WIDTH * 2);
		// i genuinely do not know how the fuck this is 4 font width wider than it should be
		length1 -= FONT_WIDTH * 2;
		display.setCursor(Point(
			(DISPLAY_WIDTH - length1) >> 1, 
			(DISPLAY_HEIGHT >> 1) - FONT_HEIGHT)
		);
		display.print(measurement, Colors::White, 1, 2);
		display.print("C", Colors::White, 2);
	}

	if((prevTemp1 >= 100 && measurement < 100) || 
		(prevTemp2 >= 100 && target < 100) || 
		(prevTemp1 < 100 && measurement >= 100) || 
		(prevTemp2 < 100 && target >= 100))
		display.fill(Colors::RaspberryRed);

	int length2 = (display.getStringLength(target) + FONT_WIDTH);
	display.setCursor(Point(
		(DISPLAY_WIDTH - length2) >> 1, 
		(DISPLAY_HEIGHT >> 1) + FONT_HEIGHT)
	);
	display.print(target, Colors::DarkGrey, 2, 1);
	display.print("C", Colors::DarkGrey, 1);

	prevTemp1 = measurement;
	prevTemp2 = target;

	// fill the screen once every 5 seconds using millis
	if((time_us_32() - lastUpdate) > 5000000)
	{
		display.fill(Colors::RaspberryRed);
		lastUpdate = time_us_32();
	}
}