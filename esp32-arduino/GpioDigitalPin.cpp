#include "../GpioDigitalPin.h"

#include "GpioPinDefinition.h"
#include "esp32-hal-gpio.h"

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

void GpioDigitalPin::init(GpioDigitalPin::Direction type) const
{
    pinMode(gpioPin.pin, type == Direction::Input ? INPUT : OUTPUT);

}

}