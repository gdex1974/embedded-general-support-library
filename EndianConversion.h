#pragma once

#include <cstdint>

namespace embedded
{

inline constexpr uint16_t changeEndianess(const uint16_t in)
{
    return ((in & 0xFF00) >> 8) | ((in & 0x00FF) << 8);
}

inline constexpr uint32_t changeEndianess(const uint32_t in)
{
    return ((in & 0xff000000ul) >> 24) | ((in & 0x00FF0000ul) >> 8) | ((in & 0x0000FF00ul) << 8) |
           ((in & 0xFFul) << 24);
}

inline constexpr float changeEndianessToFloat(const uint32_t bytes)
{
    union
    {
        uint32_t uintValue;
        float floatValue;
    } conversion { changeEndianess(bytes) };
    return conversion.floatValue;
}

inline void changeEndianess16x2(uint32_t* bytes)
{
    auto in = *bytes;
    in = ((in & 0xff000000) >> 24) | ((in & 0x00FF0000) >> 8) | ((in & 0x0000FF00) << 8) | ((in & 0xFF) << 24);
    *bytes = (in >> 16) | (in << 16);
}

}
