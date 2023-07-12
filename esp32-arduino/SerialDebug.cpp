#ifdef DEBUG_SERIAL_OUT
#include "SerialOut.h"
#include "PacketUartImpl.h"

namespace
{
    embedded::PacketUart uart1(static_cast<embedded::PacketUart::UartDevice&>(Serial));
    std::array<char, 128> serialBuffer;
}
namespace embedded
{
SerialOut SerialDebug(serialBuffer, uart1);
}
#endif