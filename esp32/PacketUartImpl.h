#pragma once

#include "../PacketUart.h"
#include <HardwareSerial.h>

class PacketUart::UartDevice : public HardwareSerial
{
public:
    using HardwareSerial::HardwareSerial;
};