#pragma once

#include <cstdint>

struct spi_device_t;

namespace embedded
{

class GpioPinDefinition;

class SpiBus
{
public:
    enum class DuplexMode
    {
        Half,
        Full
    };
    explicit SpiBus(int busNumber) : spiBusNumber(busNumber) {}
    ~SpiBus();
    bool init(GpioPinDefinition &sck, GpioPinDefinition &miso, GpioPinDefinition &mosi,
              uint32_t frequency = 4000000, DuplexMode duplexMode = DuplexMode::Half);
    bool release();
    bool transfer(const uint8_t* sendBytes, uint8_t* receiveBytes, uint32_t length);

private:
    int spiBusNumber;
    spi_device_t* spiDevice = nullptr;
    uint32_t maxTransferSize = 0;
};

}
