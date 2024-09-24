#include "SimpleUartDevice.h"
#include <hal/uart_types.h>
#include <driver/uart.h>

namespace embedded
{
static_assert((uint8_t)SimpleUartDevice::DataBits::Bits5 == UART_DATA_5_BITS);
static_assert((uint8_t)SimpleUartDevice::DataBits::Bits6 == UART_DATA_6_BITS);
static_assert((uint8_t)SimpleUartDevice::DataBits::Bits7 == UART_DATA_7_BITS);
static_assert((uint8_t)SimpleUartDevice::DataBits::Bits8 == UART_DATA_8_BITS);

static_assert((uint8_t)SimpleUartDevice::StopBits::Bits1 == UART_STOP_BITS_1);
static_assert((uint8_t)SimpleUartDevice::StopBits::Bit1_5 == UART_STOP_BITS_1_5);
static_assert((uint8_t)SimpleUartDevice::StopBits::Bits2 == UART_STOP_BITS_2);

static_assert((uint8_t)SimpleUartDevice::ParityCheck::None == UART_PARITY_DISABLE);
static_assert((uint8_t)SimpleUartDevice::ParityCheck::Even == UART_PARITY_EVEN);
static_assert((uint8_t)SimpleUartDevice::ParityCheck::Odd == UART_PARITY_ODD);

static_assert((uint8_t)SimpleUartDevice::FlowControl::Disable == UART_HW_FLOWCTRL_DISABLE);
static_assert((uint8_t)SimpleUartDevice::FlowControl::RTS == UART_HW_FLOWCTRL_RTS);
static_assert((uint8_t)SimpleUartDevice::FlowControl::CTS == UART_HW_FLOWCTRL_CTS);
static_assert((uint8_t)SimpleUartDevice::FlowControl::RTS_CTS == UART_HW_FLOWCTRL_CTS_RTS);

bool SimpleUartDevice::init(int uartNumber,
                            int rxPin,
                            int txPin,
                            int baudRate,
                            SimpleUartDevice::DataBits dataBits,
                            SimpleUartDevice::ParityCheck parityCheck,
                            SimpleUartDevice::StopBits stopBits,
                            SimpleUartDevice::FlowControl flowCtrl)
{
    const auto uartNum = static_cast<uart_port_t>(uartNumber);
    if (uart_is_driver_installed(uartNum))
    {
        return true;
    }
    uart_config_t uartConfig = {
            .baud_rate = baudRate,
            .data_bits = static_cast<uart_word_length_t>(dataBits),
            .parity = static_cast<uart_parity_t>(parityCheck),
            .stop_bits = static_cast<uart_stop_bits_t>(stopBits),
            .flow_ctrl = static_cast<uart_hw_flowcontrol_t>(flowCtrl),
            .rx_flow_ctrl_thresh = static_cast<uint8_t>((flowCtrl == FlowControl::Disable ? 0 : 122)),
            .source_clk = UART_SCLK_APB,
#if __GNUC__ > 8
            .flags = {}
#endif
    };
    ESP_ERROR_CHECK(uart_param_config(uartNum, &uartConfig));
    ESP_ERROR_CHECK(uart_set_pin(uartNum, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
#ifdef CONFIG_UART_ISR_IN_IRAM
    constexpr int intrAllocFlags = ESP_INTR_FLAG_IRAM;
#else
    constexpr int intrAllocFlags = 0;
#endif
    ESP_ERROR_CHECK(uart_driver_install(uartNum, UART_FIFO_LEN * 2, 0, 0, nullptr, intrAllocFlags));

    return true;
}

int SimpleUartDevice::sendBlocking(const uint8_t* data, std::size_t size) const
{
    auto result = uart_write_bytes(uartNum, (const char *) data, size);
    if (result > 0)
    {
        uart_wait_tx_done(uartNum, portMAX_DELAY);
    }
    return result;
}

int SimpleUartDevice::receiveBlocking(uint8_t* data, std::size_t size, uint32_t timeoutMilliseconds) const
{
    const auto delayTicks = timeoutMilliseconds / portTICK_PERIOD_MS;
    return uart_read_bytes(uartNum, data, size, delayTicks);
}

} // embedded
