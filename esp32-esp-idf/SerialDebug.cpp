#ifdef DEBUG_SERIAL_OUT
#include "SerialOut.h"
#include "PacketUart.h"
#include "PacketUartImpl.h"

namespace
{
    embedded::PacketUart::UartDevice uartDevice(0);
    embedded::PacketUart uart1(uartDevice);
    std::array<char, 128> serialBuffer;
}
namespace embedded
{
SerialOut SerialDebug(serialBuffer, uart1);
}
#endif