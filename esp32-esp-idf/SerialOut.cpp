#include "../SerialOut.h"
#include "PacketUartImpl.h"

void embedded::SerialOut::flush()
{
    auto dataView = bufferedOut.data();
    uartDevice.Send(dataView.begin(), dataView.size(), 100);
    bufferedOut.clear();
}
