#include "../GpioDigitalPin.h"

#include "GpioPinDefinition.h"
#include <Arduino.h>

namespace embedded
{

void GpioDigitalPin::set() const
{
    digitalWrite(gpioPin.pin, HIGH);
}

void GpioDigitalPin::reset() const
{
    digitalWrite(gpioPin.pin, LOW);
}

bool GpioDigitalPin::check() const
{
    return digitalRead(gpioPin.pin) != 0;
}

bool GpioDigitalPin::init(GpioDigitalPin::Direction type, GpioDigitalPin::PullMode mode) const
{
    uint8_t pinType = type == Direction::Input ? INPUT : OUTPUT;
    if (mode == PullMode::Up)
    {
        pinType |= PULLUP;
    }
    else if (mode == PullMode::Down)
    {
        pinType |= PULLDOWN;
    }
    pinMode(gpioPin.pin, pinType);
    return true;
}

}