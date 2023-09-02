#pragma once

#include <Arduino.h>
#include <SPI.h>

namespace embedded
{

class SpiBus
{
public:
    explicit SpiBus(SPIClass &bus, uint32_t frequency = 4000000) :
            spi(bus), spiSettings(frequency, MSBFIRST, SPI_MODE0) {}

    bool transfer(const uint8_t* in, uint8_t* out, uint16_t length);

    void sendByte(uint8_t data)
    {
        spi.write(data);
    }

    void sendBytes(const uint8_t* data, uint16_t length)
    {
        spi.writeBytes(data, length);
    }

private:
    SPIClass &spi;
    SPISettings spiSettings;
};

}
