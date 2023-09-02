#include "../I2CDevice.h"
#include "I2CBus.h"

bool embedded::I2CDevice::sendSync(const uint8_t *data, uint16_t count) const
{
    impl.i2c.beginTransmission(deviceAddress);
    auto result = impl.i2c.write((uint8_t*)data, count);
    return (impl.i2c.endTransmission(true) == 0) && result == count;
}

bool embedded::I2CDevice::receiveSync(uint8_t *data, uint16_t count) const
{
    if (size_t recv = impl.i2c.requestFrom((int)deviceAddress, (int)count); recv != count)
    {
        return false;
    }

    for (uint16_t i = 0; i < count; ++i) {
        data[i] = impl.i2c.read();
    }
    return true;
}

