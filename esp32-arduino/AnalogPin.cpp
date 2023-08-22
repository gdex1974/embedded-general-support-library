#include "../AnalogPin.h"
#include "GpioPinDefinition.h"
#include "esp32-hal-adc.h"

int embedded::AnalogPin::singleRead() const
{
    return analogRead(gpioPin.pin);
}
