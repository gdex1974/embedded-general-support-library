#pragma once

#include <iosfwd>
#include <cstdint>

template <typename Stream>
class BitStream
{
public:
    explicit BitStream(Stream &stream) : stream(stream) {}
    BitStream& operator<<(bool bit)
    {
        if (bit)
        {
            buffer |= 1 << (7 - bitCount);
        }
        if (++bitCount == 8)
        {
            stream << buffer;
            buffer = 0;
            bitCount = 0;
        }
        return *this;
    }

    BitStream& operator<<(uint8_t byte)
    {
        if (bitCount == 0)
        {
            stream << byte;
        }
        else
        {
            buffer |= byte >> bitCount;
            stream << buffer;
            buffer = byte << (8 - bitCount);
        }
        return *this;
    }

    void flush()
    {
        if (bitCount != 0)
        {
            stream << buffer;
            buffer = 0;
            bitCount = 0;
        }
    }
private:
    Stream &stream;
    int bitCount = 0;
    uint8_t buffer = 0;
};
