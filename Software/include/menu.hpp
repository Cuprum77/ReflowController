#pragma once

#include "pico/stdlib.h"
#include "defines.hpp"

// Menu states
enum menuState_t
{
	menuTemperature,
	menuSetPoint,
	menuTimer,
	menuSettings,
};

class Menu
{
public:
	// Constructor
	Menu(PicoGFX* display, Oven* oven, RotaryEncoder* encoder, FontStruct* fontBig, FontStruct* fontSmall);

	// Handle the gauges seperately to both allow finer control, and prevent dynamic memory allocation
	void attachTemperatureGauge(DialGauge* gauge);
	void attachSetPointGauge(DialGauge* gauge);

	// Draw the menu
	error_state_t update(menuState_t menu);
	int getSetPoint();

private:
	PicoGFX* display = nullptr;
	Oven* oven = nullptr;
	RotaryEncoder* encoder = nullptr;
	FontStruct* fontBig = nullptr; 
	FontStruct* fontSmall = nullptr;
	DialGauge* temperatureGauge = nullptr;
	DialGauge* setPointGauge = nullptr;
	int temperature = 0, setPoint = 50;

	error_state_t drawTemperatureGauge();
	error_state_t drawSetPointMenu();
	error_state_t drawTimer();
	error_state_t drawSettingsMenu();
};