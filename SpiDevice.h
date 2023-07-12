#pragma once

#include <cstdint>

namespace embedded
{
class SpiBus;

class SpiDevice
{
public:
    explicit SpiDevice(SpiBus &bus) : spiBus(bus) {}

    bool sendSync(const uint8_t* data, uint16_t length, uint32_t timeout = 1000) const;
    bool receiveSync(uint8_t* data, uint16_t length, uint32_t timeout = 1000) const;

    bool sendThenReceive(const uint8_t* txData,
                         uint16_t txCount,
                         uint8_t* rxData,
                         uint16_t rxCount,
                         uint32_t timeoutMilliseconds = 1000) const
    {
        return sendSync(txData, txCount, timeoutMilliseconds) && receiveSync(rxData, rxCount, timeoutMilliseconds);
    }

    bool sendAndReceive(const uint8_t* txData, uint8_t* rxData, uint16_t size, uint32_t timeout = 1000) const;

private:
    SpiBus &spiBus;
};

}
