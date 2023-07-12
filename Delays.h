#pragma once

#include <cstdint>

namespace embedded
{
    void ligntSleep(uint32_t milliseconds);
    void delay(uint32_t milliseconds);
    void delayMicroseconds(uint32_t microseconds);
    uint32_t getMillisecondTicks();//milliseconds
};
