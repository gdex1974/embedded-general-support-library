#pragma once

#include <cstdint>
#include <hal/uart_types.h>

namespace embedded
{

class SimpleUartDevice
{
public:
    enum class DataBits : uint8_t {
        Bits5 = 0,
        Bits6 = 1,
        Bits7 = 2,
        Bits8 = 3,
    };

    enum class StopBits : uint8_t {
        Bits1 = 1,
        Bit1_5 = 2,
        Bits2 = 3,
    };

    enum class ParityCheck : uint8_t {
        None = 0,
        Even = 2,
        Odd = 3,
    };

    enum class FlowControl : uint8_t {
        Disable = 0,
        RTS = 1,
        CTS = 2,
        RTS_CTS = 3,
    };

    explicit SimpleUartDevice(int uartNum) : uartNum(uartNum) {}
    static bool init(int uartNum, int rxPin, int txPin, int baudRate,
                     DataBits dataBits = DataBits::Bits8, ParityCheck parityCheck = ParityCheck::None,
                     StopBits stopBits = StopBits::Bits1, FlowControl flowCtrl = FlowControl::Disable);
    int sendBlocking(const uint8_t* data, std::size_t size) const;
    int receiveBlocking(uint8_t* data, std::size_t size, uint32_t timeoutMilliseconds = 20) const;

private:
    uart_port_t uartNum;
};

} // embedded
