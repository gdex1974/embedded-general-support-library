#include "../SpiDevice.h"
#include "SpiBus.h"

namespace embedded
{

bool SpiDevice::sendSync(const uint8_t* data, uint16_t length, uint32_t /*timeout*/) const
{
    return spiBus.transfer(data, nullptr, length);
}

bool SpiDevice::receiveSync(uint8_t* data, uint16_t length, uint32_t /*timeout*/) const
{
    return spiBus.transfer(nullptr, data, length);
}

bool SpiDevice::sendAndReceive(const uint8_t* txData, uint8_t* rxData, uint16_t size, uint32_t /*timeout*/) const
{
    return spiBus.transfer(txData, rxData, size);
}

}
