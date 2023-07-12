#include <charconv>
#include <cstdlib>
#include "BufferedOut.h"

namespace
{

char* toChars(char* first, char* last, float val, unsigned int precision) noexcept
{
    int n = int(val);
    auto [pos, ec] = std::to_chars(first, last, n);
    if (ec == std::errc {} && precision > 0 && pos < last)
    {
        float f = std::abs(val) - std::abs(float(n));
        *(pos++) = '.';
        while (precision > 0 && pos != last)
        {
            f *= 10;
            n = int(f);
            *(pos++) = char('0' + n);
            f -= float(n);
            --precision;
        }
    }
    return pos;
}

char* append(char* dst, const char* end, const char* str)
{
    while (*str && dst != end)
    {
        *(dst++) = *(str++);
    }
    return dst;
}

} // namespace

namespace embedded
{

BufferedOut &BufferedOut::operator<<(const char* str)
{
    pos = append(pos, dataBuf.end(), str);
    return *this;
}


BufferedOut &BufferedOut::operator<<(int n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    pos = p;
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned int n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    pos = p;
    return *this;
}

BufferedOut &BufferedOut::operator<<(long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    pos = p;
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    pos = p;
    return *this;
}

BufferedOut &BufferedOut::operator<<(long long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    pos = p;
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned long long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    pos = p;
    return *this;
}

BufferedOut &BufferedOut::operator<<(float f)
{
    pos = toChars(pos, dataBuf.end(), f, floatPrecision);
    return *this;
}

BufferedOut &BufferedOut::operator<<(BytesView span)
{
    char digits[] = "0123456789ABCDEF";
    const auto availableSize = (dataBuf.end() - pos) / 2;
    auto size = span.size() < availableSize ? span.size() : availableSize;
    for (int i = 0; i < size; ++i)
    {
        unsigned digit = span[i];
        *(pos++) = digits[(digit & 0xF0u) >> 4];
        *(pos++) = digits[digit & 0xFu];
    }
    return *this;
}

} // namespace embedded
