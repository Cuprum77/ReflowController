#pragma once
#include "Memory_Registers.hpp"

// TS391AX
ReflowProfile Profile1 =
{
    1,      // profile number
    100,    // 100°C (Preheat target)
    150,    // 150°C (Soak target)
    183,    // 183°C (Reflow target)
    235,    // 235°C (Peak target)
    30,     // 30 seconds (Preheat time)
    120,    // 120 seconds (Soak time)
    150,    // 150 seconds (Reflow time)
    210,    // 210 seconds (Peak time)
    240,    // 240 seconds (Cool time)
};