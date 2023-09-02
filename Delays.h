#pragma once

#include <cstdint>

namespace embedded
{
    void delay(uint32_t milliseconds);
    void delayMicroseconds(uint32_t microseconds);
    uint32_t getMillisecondTicks();//milliseconds
    uint64_t getMicrosecondTicks();//microseconds
};
