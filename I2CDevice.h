#pragma once

#include <cstdint>

namespace embedded
{

struct I2CBus;

class I2CDevice
{
public:
    I2CDevice(I2CBus &bus, uint16_t address, uint32_t maxWaitMs = 200)
            : impl(bus), deviceAddress(address), timeout(maxWaitMs) {}

    bool sendSync(const uint8_t* data, uint16_t count) const;
    bool receiveSync(uint8_t* data, uint16_t count) const;

    bool sendThenReceive(const uint8_t* txData, uint16_t txCount, uint8_t* rxData, uint16_t rxCount) const
    {
        return sendSync(txData, txCount) && receiveSync(rxData, rxCount);
    }

protected:
    uint16_t getAddress() const { return deviceAddress; }

private:
    I2CBus &impl;
    uint16_t deviceAddress;
    uint32_t timeout;
};

}
