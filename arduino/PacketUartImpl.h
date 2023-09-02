#pragma once

#include "../PacketUart.h"
#include <HardwareSerial.h>

class embedded::PacketUart::UartDevice : public HardwareSerial
{
public:
    using HardwareSerial::HardwareSerial;
};