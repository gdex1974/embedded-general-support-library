#pragma once

#include <cstdint>

namespace embedded
{

struct GpioPinDefinition;

class AnalogPin
{
public:
    explicit AnalogPin(GpioPinDefinition &pin) : gpioPin(pin) {}
    int singleRead() const;

private:
    GpioPinDefinition &gpioPin;
};
}
