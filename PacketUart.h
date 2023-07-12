#pragma once

#include <cstdint>

namespace embedded
{

class PacketUart
{
public:
    class UartDevice;

    explicit PacketUart(UartDevice &uart);
    uint16_t Receive(uint8_t* buffer, uint16_t bufferSize, uint16_t timeout = 200);
    uint16_t ReceiveUntil(uint8_t* buffer, uint16_t bufferSize, uint8_t endByte, uint16_t timeoutMilliseconds = 0);
    uint16_t Send(const uint8_t* buffer, uint16_t bufferSize, uint32_t timeoutMilliseconds = 100) const;
private:
    UartDevice &uartDevice;
};

}
