#include "SpiBus.h"

namespace
{
    void read(SPIClass& spi, uint8_t* out, uint16_t size)
    {
        switch (size)
        {
            case 3:
                *out++ = spi.transfer((uint8_t)0xFFu);
                [[fallthrough]];
            case 2:
                *out++ = spi.transfer((uint8_t)0xFFu);
                [[fallthrough]];
            case 1:
                *out = spi.transfer((uint8_t)0xFFu);
                [[fallthrough]];
            case 0:
                return;
            default:
            {
                const auto dwords = size / 4;
                spi.transferBytes(nullptr, out, dwords * 4);
                if (uint16_t rest = size % 4)
                {
                    read(spi, out + dwords * 4, rest);
                }
            }
        }
    }

    void write(SPIClass& spi, const uint8_t* in, uint16_t size)
    {
        switch (size)
        {
            case 3:
                spi.transfer(*in++);
                [[fallthrough]];
            case 2:
                 spi.transfer(*in++);
                [[fallthrough]];
            case 1:
                 spi.transfer(*in);
                [[fallthrough]];
            case 0:
                return;
            default:
            {
                const auto dwords = size / 4;
                spi.transferBytes(in, nullptr, dwords * 4);
                if (uint16_t rest = size % 4)
                {
                    write(spi, in + dwords * 4, rest);
                }
                return;
            }
        }
    }

    void readAndWrite(SPIClass& spi, const uint8_t* in, uint8_t* out, uint16_t size)
    {
        switch (size)
        {
            case 3:
                *out++ = spi.transfer(*in++);
                [[fallthrough]];
            case 2:
                *out++ = spi.transfer(*in++);
                [[fallthrough]];
            case 1:
                *out = spi.transfer(*in);
                [[fallthrough]];
            case 0:
                return;
            default:
            {
                const auto dwords = size / 4;
                spi.transferBytes(in, out, dwords * 4);
                if (uint16_t rest = size % 4)
                {
                    readAndWrite(spi, in + dwords * 4, out + dwords * 4, rest);
                }
                return;
            }
        }
    }
}

bool embedded::SpiBus::transfer(const uint8_t *in, uint8_t *out, uint16_t length)
{
    spi.beginTransaction(spiSettings);
    if (in == nullptr)
    {
        if (out != nullptr)
            read(spi, out, length);
    }
    else if (out == nullptr)
    {
        write(spi, in, length);
    }
    else
    {
        readAndWrite(spi, in, out, length);
    }
    spi.endTransaction();
    return true;
}
