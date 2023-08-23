#pragma once

#include <cstdint>

namespace embedded
{
class SpiBus;

class SpiDevice
{
public:
    explicit SpiDevice(SpiBus &bus) : spiBus(bus) {}

    // Work only in half duplex mode
    bool sendSync(const uint8_t* data, uint16_t length, uint32_t timeout = 1000) const;
    bool receiveSync(uint8_t* data, uint16_t length, uint32_t timeout = 1000) const;

    // Work in full duplex mode
    bool sendAndReceive(const uint8_t* txData, uint8_t* rxData, uint16_t size, uint32_t timeout = 1000) const;

private:
    SpiBus &spiBus;
};

}
