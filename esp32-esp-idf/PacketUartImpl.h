#pragma once

#include "../PacketUart.h"
#include "SimpleUartDevice.h"

class embedded::PacketUart::UartDevice : public embedded::SimpleUartDevice
{
public:
    using embedded::SimpleUartDevice::SimpleUartDevice;
};