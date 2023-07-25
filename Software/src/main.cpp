#include "include/Defines.hpp"

// set the display parameters
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
Gradients gradients(display.getFrameBuffer(), displayParams);
// Create the encoder/decoder object
Encoder encoder;
// Create the PicoGFX object
PicoGFX picoGFX(&display, &print, &graphics, &gradients, &encoder);

// initialize the EEPROM and MCP9600 ICs
Memory memory(EEPROM_ADDRESS, i2c0);
MCP9600 mcp9600_1(MCP9600_1_ADDRESS, i2c0);
MCP9600 mcp9600_2(MCP9600_2_ADDRESS, i2c0);
MCP9600 mcp9600_3(MCP9600_3_ADDRESS, i2c0);
PID pid(1.25f, 0.0f, 0.0f, 0.02f, -127.0f, 127.0f, 0.1f);

// global variables
double measurement = 0;
int measurementInt = 0;
double target = 0;
int targetInt = 0;

void runOven();
void main1();

unsigned long offTime = 0;
unsigned long onTime = 0;
unsigned long lastTriggerTime = 0;
unsigned long runOccasionallyLastTime = 0;
unsigned long blinkLastTime = 0;
bool isOn = false;
bool enabled = false;
bool blink = false;

int main()
{
	// initialize the stdio
	stdio_init_all();

	// initialize the SPI bus and display
	spi.init();
	display.init();

	// enable the led
	gpio_init(LED_BUILDIN);
	gpio_init(LED_FRONT);
	gpio_init(OUTPUT_ENABLE_1_PIN);
	gpio_init(BUTTON_PIN);
	gpio_set_dir(LED_BUILDIN, GPIO_OUT);
	gpio_set_dir(LED_FRONT, GPIO_OUT);
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

	// enable watchdog requiring a reset every 1000ms
	watchdog_enable(1000, 1);

	// setup the second core
	multicore_launch_core1(main1);
	
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
		if((time_us_64() - runOccasionallyLastTime) >= 10000)
		{
			runOven();
			runOccasionallyLastTime = time_us_32();
		}

		// blink the leds
		if((time_us_64() - blinkLastTime) >= 500000)
		{
			gpio_put(LED_BUILDIN, blink);
			gpio_put(LED_FRONT, !blink);
			blink = !blink;
			blinkLastTime = time_us_32();
		}

		watchdog_update();
	}
}

bool prevButton = true;
void runOven()
{
	measurement = mcp9600_3.getTemperature();
	target = 50.0f;
	double tempTarget = 50.0f;
	int length1 = 0;

	bool output = pid.getOutput();

	// read the button state and set the output enable pin
	bool button = gpio_get(BUTTON_PIN);
	// single shot it to avoid bouncing
	if(button && !prevButton)
		enabled = !enabled;
	prevButton = button;

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

	measurementInt = (int)measurement;
	targetInt = (int)target;
	printf("Measurement: %d, Target: %d\n", measurementInt, targetInt);
}

void main1()
{
	int radius = 200;
	double theta = 0;
	double rotationSpeed = 0.05;
	int framecounter = 0;
	int frames = 0;
	unsigned long timer = 0;

	Point center = picoGFX.getDisplay().getCenter();
	unsigned width = picoGFX.getDisplay().getWidth();
	unsigned height = picoGFX.getDisplay().getHeight();

	while(1)
	{
		picoGFX.getGradients().drawRotCircleGradient(center, 120, 10, Colors::Blue, Colors::Derg);
		//picoGFX.getGradients().fillGradient(Colors::Blue, Colors::Derg, {0,0}, {0, height});
		//picoGFX.getDisplay().fill(Colors::Black);

		// Handle the hot junction temperature printing
		picoGFX.getPrint().setFont(&ComicSans48);
		picoGFX.getPrint().setColor(Colors::White);
		picoGFX.getPrint().setCursor(Point(
			(width - (picoGFX.getPrint().getStringLength(measurementInt) + picoGFX.getPrint().getStringLength(" C"))) >> 1, 
			(height >> 1) - 48)
		);
		picoGFX.getPrint().print(measurementInt);
		picoGFX.getPrint().print(" C");

		// Handle the ambient temperature printing
		picoGFX.getPrint().setFont(&ComicSans24);
		picoGFX.getPrint().setColor(Colors::DarkGrey);
		picoGFX.getPrint().setCursor(Point(
			(width - (picoGFX.getPrint().getStringLength(targetInt) + picoGFX.getPrint().getStringLength(" C"))) >> 1, 
			(height >> 1) + 12)
		);
		picoGFX.getPrint().print(targetInt);
		picoGFX.getPrint().print(" C");

		// Handle the frame counter printing
		picoGFX.getPrint().setColor(Colors::GreenYellow);
		picoGFX.getPrint().setCursor(Point((width - (picoGFX.getPrint().getStringLength(frames) + picoGFX.getPrint().getStringLength(" fps"))) >> 1, 0));
		picoGFX.getPrint().print(frames);
		picoGFX.getPrint().print(" fps");

		// Update the display
		picoGFX.getDisplay().update();

		// Update the frame counter
		framecounter++;
		if((time_us_64() - timer) >= 1000000)
		{
			timer = time_us_64();
			frames = framecounter;
			framecounter = 0;
		}
	}
}