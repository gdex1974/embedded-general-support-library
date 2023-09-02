#include "PacketUartImpl.h"

#include <algorithm>

namespace embedded
{

static_assert(sizeof(HardwareSerial) == sizeof(PacketUart::UartDevice));
static_assert(std::is_convertible_v<PacketUart::UartDevice, HardwareSerial>);;

uint16_t PacketUart::Receive(uint8_t* buffer, uint16_t bufferSize, uint16_t timeout)
{
    auto startTime = millis();
    auto maxTime = startTime + timeout;
    uint16_t currentPos = 0;
    while (millis() < maxTime && currentPos < bufferSize)
    {
        int nextByte = uartDevice.read();
        if (nextByte >= 0)
        {
            buffer[currentPos++] = (uint8_t)nextByte;
        }
        else
        {
            delay(1);
        }
    }
    return currentPos;
}

uint16_t PacketUart::Send(const uint8_t* buffer, uint16_t bufferSize, uint32_t timeoutMilliseconds) const
{
    auto oldTimeout = uartDevice.getTimeout();
    if (timeoutMilliseconds > 0)
    {
        uartDevice.setTimeout(timeoutMilliseconds);
    }
    auto result = uartDevice.write(buffer, bufferSize);
    uartDevice.flush(false);
    uartDevice.setTimeout(oldTimeout);
    return result;
}

uint16_t PacketUart::ReceiveUntil(uint8_t* buffer, uint16_t bufferSize, uint8_t endByte, uint16_t timeoutMilliseconds)
{
    uint16_t currentPos = 0;
    const auto oldTimeout = uartDevice.getTimeout();
    if (timeoutMilliseconds > 0)
    {
        uartDevice.setTimeout(timeoutMilliseconds);
    }
    currentPos += uartDevice.readBytesUntil((char)endByte, buffer, bufferSize - 1);
    buffer[currentPos++] = endByte;
    uartDevice.setTimeout(oldTimeout);
    return currentPos;
}

uint16_t PacketUart::ReceiveBetween(uint8_t* buffer,
                                    uint16_t bufferSize,
                                    uint8_t startByte,
                                    uint8_t endByte,
                                    uint16_t timeoutMilliseconds)
{
    uint16_t currentPos = 0;
    auto oldTimeout = uartDevice.getTimeout();
    if (timeoutMilliseconds > 0)
    {
        uartDevice.setTimeout(timeoutMilliseconds);
    }
    auto startTime = millis();
    auto maxTime = startTime + timeoutMilliseconds;
    while (timeoutMilliseconds == 0 || millis() < maxTime)
    {
        int nextByte = uartDevice.read();
        if (nextByte == startByte)
        {
            buffer[currentPos++] = (uint8_t)nextByte;
            currentPos += uartDevice.readBytesUntil((char)endByte, buffer + currentPos, bufferSize - currentPos - 1);
            buffer[currentPos++] = endByte;
            break;
        }
    }
    uartDevice.setTimeout(oldTimeout);
    return currentPos;
}

}
