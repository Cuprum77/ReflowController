#pragma once

#include "Color.h"
#include <stddef.h>

__attribute__((section(".heatmapTable"))) static Color heatmap[] __attribute__((aligned(4))) =
{
    (unsigned short)0x001f, (unsigned short)0x007f, (unsigned short)0x00ff, (unsigned short)0x017f, (unsigned short)0x01ff,
    (unsigned short)0x027f, (unsigned short)0x02ff, (unsigned short)0x037f, (unsigned short)0x03ff, (unsigned short)0x047f,
    (unsigned short)0x04ff, (unsigned short)0x057f, (unsigned short)0x05ff, (unsigned short)0x067f, (unsigned short)0x06ff,
    (unsigned short)0x077f, (unsigned short)0x07ff, (unsigned short)0x0ffe, (unsigned short)0x1ffc, (unsigned short)0x2ffa,
    (unsigned short)0x3ff8, (unsigned short)0x4ff6, (unsigned short)0x5ff4, (unsigned short)0x6ff2, (unsigned short)0x7ff0,
    (unsigned short)0x8fee, (unsigned short)0x9fec, (unsigned short)0xafea, (unsigned short)0xbfe8, (unsigned short)0xcfe6,
    (unsigned short)0xdfe4, (unsigned short)0xefe2, (unsigned short)0xffe0, (unsigned short)0xffe0, (unsigned short)0xffc0,
    (unsigned short)0xffa0, (unsigned short)0xff80, (unsigned short)0xff60, (unsigned short)0xff40, (unsigned short)0xff20,
    (unsigned short)0xff00, (unsigned short)0xfee0, (unsigned short)0xfec0, (unsigned short)0xfea0, (unsigned short)0xfe80,
    (unsigned short)0xfe60, (unsigned short)0xfe60, (unsigned short)0xfe40, (unsigned short)0xfe20, (unsigned short)0xfe00,
    (unsigned short)0xfde0, (unsigned short)0xfdc0, (unsigned short)0xfda0, (unsigned short)0xfd80, (unsigned short)0xfd60,
    (unsigned short)0xfd40, (unsigned short)0xfd20, (unsigned short)0xfd00, (unsigned short)0xfce0, (unsigned short)0xfcc0,
    (unsigned short)0xfcc0, (unsigned short)0xfca0, (unsigned short)0xfc80, (unsigned short)0xfc60, (unsigned short)0xfc40,
    (unsigned short)0xfc20, (unsigned short)0xfc00, (unsigned short)0xfbe0, (unsigned short)0xfbc0, (unsigned short)0xfba0,
    (unsigned short)0xfb80, (unsigned short)0xfb60, (unsigned short)0xfb40, (unsigned short)0xfb40, (unsigned short)0xfb20,
    (unsigned short)0xfb00, (unsigned short)0xfae0, (unsigned short)0xfac0, (unsigned short)0xfaa0, (unsigned short)0xfa80,
    (unsigned short)0xfa60, (unsigned short)0xfa40, (unsigned short)0xfa20, (unsigned short)0xfa00, (unsigned short)0xf9e0,
    (unsigned short)0xf9c0, (unsigned short)0xf9a0, (unsigned short)0xf9a0, (unsigned short)0xf980, (unsigned short)0xf960,
    (unsigned short)0xf940, (unsigned short)0xf920, (unsigned short)0xf900, (unsigned short)0xf8e0, (unsigned short)0xf8c0,
    (unsigned short)0xf8a0, (unsigned short)0xf880, (unsigned short)0xf860, (unsigned short)0xf840, (unsigned short)0xf820,
};

static size_t heatmapSize = (sizeof(heatmap) / sizeof(heatmap[0]));
