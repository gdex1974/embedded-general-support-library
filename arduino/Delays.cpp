#include "Delays.h"

#include <Arduino.h>

void embedded::delay(uint32_t milliseconds)
{
    ::delay(milliseconds);
}

uint32_t embedded::getMillisecondTicks()
{
    return ::millis();
}

void embedded::delayMicroseconds(uint32_t microseconds)
{
    ::delayMicroseconds(microseconds);
}

uint64_t embedded::getMicrosecondTicks()
{
    return ::micros();
}
