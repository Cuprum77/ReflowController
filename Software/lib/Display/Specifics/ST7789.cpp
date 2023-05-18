#include "Display.hpp"

void Display::ST7789_Init()
{
    sleep_ms(100);

    // set the SPI format
    spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // reset the display
    this->writeData(Display_Commands::SWRESET, NULL, 0);
    sleep_ms(150);

    // set the display to sleep out
    this->writeData(Display_Commands::SLPOUT, NULL, 0);
    sleep_ms(50);

    // set the display to interface pixel format
    // 0x5 << 4 = 65k of rgb interface
    // 0x5 = 16 bits per pixel
    uchar pixelFormat = 0x5 << 4 | 0x5;
    this->writeData(Display_Commands::COLMOD, &pixelFormat, 1);
    sleep_ms(10);

    // madctl = memory access control
    this->ST7789_SetRotation((displayRotation_t)this->params.rotation);

    // set the display to memory access control
    this->setCursor({0, 0});

    // display inversion on
    this->writeData(Display_Commands::INVON, NULL, 0);
    sleep_ms(10);

    // normal display mode on
    this->writeData(Display_Commands::NORON, NULL, 0);
    sleep_ms(10);

    // display on
    this->writeData(Display_Commands::DISPON, NULL, 0);
    sleep_ms(10);
}

void Display::ST7789_SetRotation(displayRotation_t rotation)
{
    uint width = this->params.width;
	uint height = this->params.height;
	switch(rotation)
    {
        case displayRotation_t::DEG_90:
			this->writeData(MADCTL, (uchar)(Display_MADCTL::MX | Display_MADCTL::MV | Display_MADCTL::RGB));
			this->params.height = width;
			this->params.width = height;
            break;
        case displayRotation_t::DEG_180:
			this->writeData(MADCTL, (uchar)(Display_MADCTL::MX | Display_MADCTL::MY | Display_MADCTL::RGB));
			this->params.height = height;
			this->params.width = width;
            break;
        case displayRotation_t::DEG_270:
			this->writeData(MADCTL, (uchar)(Display_MADCTL::MV | Display_MADCTL::MY | Display_MADCTL::RGB));
			this->params.height = width;
			this->params.width = height;
            break;
        case displayRotation_t::DEG_0:
        default:
            this->writeData(MADCTL, (uchar)Display_MADCTL::RGB);
			this->params.height = height;
			this->params.width = width;
            break;
    }
}