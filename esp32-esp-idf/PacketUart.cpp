#include "PacketUart.h"
#include "PacketUartImpl.h"

#include "Delays.h"
#include <algorithm>

namespace embedded
{

uint16_t PacketUart::Receive(uint8_t* buffer, uint16_t bufferSize, uint16_t timeout)
{
    return uartDevice.receiveBlocking(buffer, bufferSize, timeout);
}

uint16_t PacketUart::Send(const uint8_t* buffer, uint16_t bufferSize, uint32_t /*timeoutMilliseconds*/) const
{
    return uartDevice.sendBlocking(buffer, bufferSize);
}

uint16_t PacketUart::ReceiveUntil(uint8_t* buffer, uint16_t bufferSize, uint8_t endByte, uint16_t timeoutMilliseconds)
{
    uint16_t currentPos = 0;
    auto startTime = embedded::getMillisecondTicks();
    auto maxTime = startTime + timeoutMilliseconds;
    while ( currentPos < bufferSize
            && (timeoutMilliseconds == 0 || (startTime = embedded::getMillisecondTicks()) < maxTime))
    {
        if (uartDevice.receiveBlocking(buffer + currentPos, 1, maxTime - startTime) == 1)
        {
            if (buffer[currentPos++] == endByte)
            {
                break;
            }
        }
    }

    return currentPos;
}

uint16_t PacketUart::ReceiveBetween(uint8_t* buffer,
                                    uint16_t bufferSize,
                                    uint8_t startByte,
                                    uint8_t endByte,
                                    uint16_t timeoutMilliseconds)
{
    if (bufferSize < 2)
        return 0;
    uint16_t currentPos = 0;
    auto startTime = embedded::getMillisecondTicks();
    auto maxTime = startTime + timeoutMilliseconds;
    while (startTime <= maxTime)
    {
        if(uartDevice.receiveBlocking(buffer + currentPos, 1, maxTime - startTime) == 1)
        {
            if (buffer[currentPos] == startByte)
                break;
        }
        else
        {
            return 0;
        }
    }
    ++currentPos;
    while ((startTime = embedded::getMillisecondTicks()) <= maxTime)
    {
        if (currentPos == bufferSize)
            break;
        if(uartDevice.receiveBlocking(buffer + currentPos, 1, maxTime - startTime) == 1)
        {
            if (buffer[currentPos++] == endByte)
                break;
        }
    }

    return currentPos;
}

}
