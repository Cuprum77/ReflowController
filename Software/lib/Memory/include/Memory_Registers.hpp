#pragma once

// as we store each profile as a struct thats 5 words, i decided to limit to 4 profile
// although realistically you only need 1 as a hobbyist
#define PROFILE_1_ADDR 0x00
#define PROFILE_2_ADDR 0x20
#define PROFILE_3_ADDR 0x40
#define PROFILE_4_ADDR 0x60



unsigned char prevProfileNumber = 0x0;
enum Profile 
{ 
    Profile1 = PROFILE_1_ADDR, 
    Profile2 = PROFILE_2_ADDR, 
    Profile3 = PROFILE_3_ADDR, 
    Profile4 = PROFILE_4_ADDR 
};

struct ReflowProfile
{
    unsigned char profileNumber = 0x0; // 0x1

    unsigned short preheatTarget = 0x0; // 0x2
    unsigned short soakTarget    = 0x0; // 0x3
    unsigned short reflowTarget  = 0x0; // 0x4
    unsigned short peakTarget    = 0x0; // 0x5

    unsigned short preheatTime   = 0x0; // 0x6
    unsigned short soakTime      = 0x0; // 0x6
    unsigned short reflowTime    = 0x0; // 0x7
    unsigned short peakTime      = 0x0; // 0x8
    unsigned short reflow2Time   = 0x0; // 0x9

    ReflowProfile() { }
    ReflowProfile(unsigned int* data)
    {
        profileNumber = (data[0] >> 0x8) & 0xff;
        preheatTarget = (data[0] >> 0x10) & 0xffff;
        soakTarget    = data[1] & 0xffff;
        reflowTarget  = (data[1] >> 0x10) & 0xffff;
        peakTarget    = data[2] & 0xffff;
        preheatTime   = (data[2] >> 0x10) & 0xffff;
        soakTime      = data[3] & 0xffff;
        reflowTime    = (data[3] >> 0x10) & 0xffff;
        peakTime      = data[4] & 0xffff;
        reflow2Time   = (data[4] >> 0x10) & 0xffff;
    }
    ReflowProfile(unsigned char profileNumber, 
        unsigned short preheatTarget, unsigned short soakTarget, 
        unsigned short reflowTarget, unsigned short peakTarget, 
        unsigned short preheatTime, unsigned short soakTime, 
        unsigned short reflowTime, unsigned short peakTime, 
        unsigned short reflow2Time)
    {
        this->profileNumber = profileNumber;
        this->preheatTarget = preheatTarget;
        this->soakTarget    = soakTarget;
        this->reflowTarget  = reflowTarget;
        this->peakTarget    = peakTarget;
        this->preheatTime   = preheatTime;
        this->soakTime      = soakTime;
        this->reflowTime    = reflowTime;
        this->peakTime      = peakTime;
        this->reflow2Time   = reflow2Time;
    }

    void getValue(unsigned int* data)
    {
        data[0] = (profileNumber << 0x8);
        data[0] |= (preheatTarget << 0x10);
        data[1] = soakTarget;
        data[1] |= (reflowTarget << 0x10);
        data[2] = peakTarget;
        data[2] |= (preheatTime << 0x10);
        data[3] = soakTime;
        data[3] |= (reflowTime << 0x10);
        data[4] = peakTime;
        data[4] |= (reflow2Time << 0x10);
    }
};

struct ReflowRegister
{
    bool _celsius = true;
    unsigned int _checksum = 0;
    ReflowProfile _value;        

    ReflowRegister() 
    {
        _value = ReflowProfile();
        _value.profileNumber = prevProfileNumber++;
    }

    ReflowRegister(ReflowProfile profile, bool celsius = true)
    {
        _celsius = celsius;
        _value = profile;
        _value.profileNumber = prevProfileNumber++;
    }

    ReflowProfile getValue() { return _value; }
    unsigned int getCheckSum() 
    {
        if(_checksum != 0) return _checksum;
        else
        {
            // calculate a new checksum based off the value
            _checksum = 0;
            _checksum += _value.profileNumber;
            _checksum += (unsigned int)_celsius;
            _checksum += _value.preheatTarget;
            _checksum += _value.soakTarget;
            _checksum += _value.reflowTarget;
            _checksum += _value.peakTarget;
            _checksum += _value.preheatTime;
            _checksum += _value.soakTime;
            _checksum += _value.reflowTime;
            _checksum += _value.peakTime;
            _checksum += _value.reflow2Time;
            return _checksum;
        }
    }
};

// we have 128 bytes to work with, 4 should only take up 56 bytes
struct MemoryMap
{
    ReflowRegister profile1 = ReflowRegister();
    ReflowRegister profile2 = ReflowRegister();
    ReflowRegister profile3 = ReflowRegister();
    ReflowRegister profile4 = ReflowRegister();

    ReflowRegister* getProfile(unsigned int num)
    {
        switch(num)
        {
            case 1:
                return &profile1;
            case 2:
                return &profile2;
            case 3:
                return &profile3;
            case 4:
                return &profile4;
            default:
                return nullptr;
        }
    }
};