#include "../AnalogPin.h"
#include "GpioPinDefinition.h"
#include "Arduino.h"

int embedded::AnalogPin::singleRead() const
{
    return analogRead(gpioPin.pin);
}
