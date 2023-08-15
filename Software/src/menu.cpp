#include "menu.hpp"

/**
 * @brief Construct a new Menu object
 * @param display Pointer to the display object
 * @param oven Pointer to the oven object
*/
Menu::Menu(PicoGFX* display, Oven* oven, RotaryEncoder* encoder, FontStruct* fontBig, FontStruct* fontSmall)
{
    this->display = display;
    this->oven = oven;
    this->encoder = encoder;
    this->fontBig = fontBig;
    this->fontSmall = fontSmall;
}

/**
 * @brief Attach the temperature gauge to the menu
 * @param temperatureGauge Pointer to the temperature gauge object
*/
void Menu::attachTemperatureGauge(DialGauge* temperatureGauge)
{
    this->temperatureGauge = temperatureGauge;
}

/**
 * @brief Attach the set point menu to the menu
 * @param setPointMenu Pointer to the set point menu object
*/
void Menu::attachSetPointGauge(DialGauge* setPointMenu)
{
    this->setPointGauge = setPointMenu;
}

/**
 * @brief Update the menu
 * @param menu The menu state to update to
 * @note This should be called in the main loop
*/
error_state_t Menu::update(menuState_t menu)
{
    // Error tracking variable
    int error = 0;

    // Grab the current temperature
    this->temperature = oven->reading();
    // If temperature is MAX_INT, there was an error
    if(this->temperature == 0x7fffffff)
    {
        error |= (int)error_state_t::SENSOR_ALL_ERROR;
        this->temperature = 0;
    }

    // Draw the menu
    switch(menu)
    {
        case menuState_t::menuTemperature:
            error |= (int)drawTemperatureGauge();
            break;
        case menuState_t::menuSetPoint:
            error |= (int)drawSetPointMenu();
            break;
        default:
            break;
    }

    // Push the display to the screen
    display->getDisplay().update();

    // Return the error state
    return (error_state_t)error;
}

/**
 * @brief Get the current set point the user has selected
 * @return The current set point
*/
int Menu::getSetPoint()
{
    return this->setPoint;
}

/**
 * @private
 * @brief Draw the temperature gauge
*/
error_state_t Menu::drawTemperatureGauge()
{
    // Error tracking variable
    int error = 0;

    // Clear the screen
    display->getGraphics().fill(Colors::Black);

    // If there are no temperature gauge attached, add that to the error
    if(this->temperatureGauge == nullptr) 
    {
        error |= (int)error_state_t::DISPLAY_NO_GAUGES_ERROR;
    }
    // Otherwise, update the temperature gauge
    else
    {
        this->temperatureGauge->update(this->temperature);
        // Apply basic anti-aliasing
        display->getGraphics().antiAliasingFilter();
    }

	// Handle the hot junction temperature printing
	display->getPrint().setFont(this->fontBig);
	display->getPrint().setColor(Colors::White);
	display->getPrint().setString("%d C", this->temperature);
	display->getPrint().center();
    int height = display->getDisplay().getHeight() >> 2;
	display->getPrint().moveCursor(0, height);
	display->getPrint().print();

    // We got this far, we can return with no error
    return error_state_t::NO_ERROR;
}

/**
 * @private
 * @brief Draw the set point menu
*/
error_state_t Menu::drawSetPointMenu()
{
    // Error tracking variable
    int error = 0;

    // Clear the screen
    display->getGraphics().fill(Colors::Black);

    // If there are no temperature gauge attached, return with an error
    if(this->setPointGauge == nullptr) 
    {
        error |= (int)error_state_t::DISPLAY_NO_GAUGES_ERROR;
    }
    // Otherwise, update the temperature gauge
    else
    {
        //this->setPointGauge->update(this->temperature);
        // Apply basic anti-aliasing
        display->getGraphics().antiAliasingFilter();
    }

    /* 
        DISPLAY CODE GOES HERE 
    */

    // We got this far, we can return with no error
    return error_state_t::NO_ERROR;
}