#pragma once

#include "Wire.h"
namespace embedded
{

struct I2CBus
{
    explicit I2CBus(TwoWire &bus) : i2c(bus) {}
    void resetBus();

    TwoWire &i2c;
};

}
