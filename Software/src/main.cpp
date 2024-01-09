#include "include/defines.hpp"

// set the display parameters
Display_Pins displayPins = {
	.rst = DISPLAY_RST,
	.dc = DISPLAY_DC,
	.cs = DISPLAY_CS,
	.sda = DISPLAY_MOSI,
	.scl = DISPLAY_SCK,
};

Hardware_Params hardwareParams = {
	.hw_interface = SPI_Interface_t::PIO_HW,
	.spi_instance = nullptr,
	.baudrate = 125000000,
};

Display_Params displayParams = {
	.type = display_type_t::GC9A01,
	.height = DISPLAY_HEIGHT,
	.width = DISPLAY_WIDTH,
	.columnOffset1 = DISPLAY_OFFSET_X0,
	.columnOffset2 = DISPLAY_OFFSET_X1,
	.rowOffset1 = DISPLAY_OFFSET_Y0,
	.rowOffset2 = DISPLAY_OFFSET_Y1,
	.rotation = DISPLAY_ROTATION
};

// Create the display object
HardwareSPI spi(displayPins, hardwareParams);
Display display(&spi, &displayPins, &displayParams);
// Create the GFX objects
Print print(display.getFrameBuffer(), displayParams);
Graphics graphics(display.getFrameBuffer(), displayParams);
// Create the PicoGFX object
PicoGFX picoGFX(&display, &print, &graphics, nullptr, nullptr);

// Create the peripheral objects
Memory memory(EEPROM_ADDRESS, i2c0);
MCP9600 mcp9600_1(MCP9600_1_ADDRESS, i2c0);
MCP9600 mcp9600_2(MCP9600_2_ADDRESS, i2c0);
MCP9600 mcp9600_3(MCP9600_3_ADDRESS, i2c0);
Button button(BUTTON_PIN, true);
RotaryEncoder encoder(ENCODER_A, ENCODER_B, ENCODER_SW, CRITICAL_TEMPERATURE);

// Create the oven objects
PID pid(DEFAULT_KP, DEFAULT_KI, DEFAULT_KD, DEFAULT_DT, DEFAULT_MIN, DEFAULT_MAX, DEFAULT_TOLERANCE);
Oven oven(&pid, &mcp9600_1, &mcp9600_2, &mcp9600_3, OUTPUT_ENABLE_1_PIN, OUTPUT_ENABLE_2_PIN, OUTPUT_ENABLE_3_PIN, LED_FRONT);
Menu menu(&picoGFX, &oven, &encoder, &ComicSans48, &ComicSans24);

// Declare variables for the timer loops
unsigned long runOccasionallyLastTime = 0;
unsigned long blinkLastTime = 0;
int targetTemperature = 0;
bool blink = false;
bool runOven = false;
bool prevButtonState = false;
bool EEPROM_isConnected = false;

// Handle the variables for the dial gauges
static Color setPointColors[2] = { Colors::White, Colors::Red };
size_t setPointColorSize = sizeof(setPointColors) / sizeof(setPointColors[0]);
Point center = picoGFX.getDisplay().getCenter();
int width = picoGFX.getDisplay().getWidth();
int height = picoGFX.getDisplay().getHeight();
int radius = picoGFX.getDisplay().getWidth() >> 1;

// Create a dial gauge object based on the display
DialGauge temperatureGauge(&graphics, width, height, center, radius, LOWEST_TEMPERATURE, 
	CRITICAL_TEMPERATURE, heatmap, heatmapSize, DialGaugeType_t::DialSimple);
// Create another dial for the set point menu
DialGauge setPointGauge(&graphics, width, height, center, radius, LOWEST_TEMPERATURE, 
	CRITICAL_TEMPERATURE, setPointColors, setPointColorSize, DialGaugeType_t::DialSimple2);

// Keep track of the menu state
menuState_t menuState = menuState_t::menuTemperature;

void main1()
{
	// Wait for the other core to be ready
	while(!multicore_fifo_pop_blocking()) tight_loop_contents();

	// Attach the temperature gauge to the menu
	menu.attachTemperatureGauge(&temperatureGauge);
	menu.attachSetPointGauge(&setPointGauge);

	while(1)
	{
		menu.update(menuState);
	}
}

int main()
{
	// initialize the stdio
	stdio_init_all();

	// initialize the SPI bus and display
	spi.init();
	display.init();
	picoGFX.getGraphics().fill(Colors::Pink);
	picoGFX.getDisplay().update();

	// Initialize the GPIO
	gpio_init(LED_BUILDIN);
	gpio_init(BUTTON_PIN);
	gpio_init(BUTTON_LED_PIN);

	// Set the direction of the GPIO
	gpio_set_dir(LED_BUILDIN, GPIO_OUT);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);
	gpio_set_dir(BUTTON_LED_PIN, GPIO_OUT);

	// initialize the I2C0 bus
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);
	i2c_init(i2c0, I2C_SPEED);
	// set the binary data to show the pins used for I2C0
	bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));

	// verify that the EEPROM is connected
	EEPROM_isConnected = memory.verifyConnection();

	// enable watchdog requiring a reset every 500ms
	watchdog_enable(500, 1);

	// setup the second core
	multicore_launch_core1(main1);

	// Initialize the oven
	oven.init(heaters_t::HEATER_3);
	// Update the watchdog just in case
	watchdog_update();

	// Tell the other core that they can start
	multicore_fifo_push_blocking(1);
	
	while(1)
	{
		// sleep for 10ms to avoid damaging the relay through PID seizures
		if((time_us_64() - runOccasionallyLastTime) >= 10000)
		{
			// Check if the button was clicked
			if(button.isClicked())
				runOven = !runOven;
			// Update the button led
			gpio_put(BUTTON_LED_PIN, runOven);

			oven.updateHeaters(runOven, 50);
			runOccasionallyLastTime = time_us_32();
		}

		// blink the leds
		if((time_us_64() - blinkLastTime) >= 500000)
		{
			gpio_put(LED_BUILDIN, !blink);
			blink = !blink;
			blinkLastTime = time_us_32();
		}

		// Update the encoder
		encoder.update();
		
		// Update the button
		button.update();

		// Kick the watchdog
		watchdog_update();
	}
}