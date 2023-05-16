#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "Memory_Registers.hpp"

#define DEFAULT_CLOCK           100000
#define DEFAULT_PAGE_SIZE       16
#define DEFAULT_EEPROM_SIZE     512
#define DEFAULT_WRITE_CYCLE     5
#define WORD_SIZE               4


class Memory
{
public:
    Memory(unsigned int device_address, i2c_inst_t* i2c);
    
    bool verifyConnection();
    int selfTest();

    void setProfile(Profile profile);
    Profile getProfile(unsigned int profile_number);
    void readProfile(Profile profile);
    void writeProfile(Profile profile);
private:
    unsigned int write_cycle_time;
    unsigned int eeprom_addr; 
    i2c_inst_t* i2c;
    MemoryMap memory_map;

    unsigned int readWord(unsigned int target_address);
    void writeWord(unsigned int target_address, unsigned int data);
};