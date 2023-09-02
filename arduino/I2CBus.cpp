#include "I2CBus.h"

void embedded::I2CBus::resetBus()
{
    uint8_t data = 0x06;
    i2c.beginTransmission(0);
    i2c.write(data);
    i2c.endTransmission();
}