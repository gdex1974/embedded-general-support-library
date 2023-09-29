#include <charconv>
#include <cstdlib>
#include <cstring>
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

void BufferedOut::addPadding(const char* p)
{
    auto realSize = p - pos;
    if (realSize < integerWidth)
    {
        memmove(pos + integerWidth - realSize, pos, realSize);
        auto paddingSize = integerWidth - realSize;
        auto paddingPos = pos;
        while (paddingSize-- > 0)
        {
            *(paddingPos++) = fillChar;
        }
    }
    pos += integerWidth;
}

BufferedOut &BufferedOut::operator<<(const char* str)
{
    pos = append(pos, dataBuf.end(), str);
    return *this;
}


BufferedOut &BufferedOut::operator<<(int n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    if (ec == std::errc {})
    {
        adjustPos(p);
    }
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned int n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    if (ec == std::errc {})
    {
        adjustPos(p);
    }
    return *this;
}

BufferedOut &BufferedOut::operator<<(long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    if (ec == std::errc {})
    {
        adjustPos(p);
    }
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    if (ec == std::errc {})
    {
        adjustPos(p);
    }
    return *this;
}

BufferedOut &BufferedOut::operator<<(long long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    if (ec == std::errc {})
    {
        adjustPos(p);
    }
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned long long n)
{
    auto [p, ec] = std::to_chars(pos, dataBuf.end(), n);
    if (ec == std::errc {})
    {
        adjustPos(p);
    }
    return *this;
}

void BufferedOut::adjustPos(char* p)
{
    if (integerWidth > 0)
    {
        addPadding(p);
        integerWidth = 0;
    }
    else
    {
        pos = p;
    }
}

BufferedOut &BufferedOut::operator<<(float f)
{
    pos = toChars(pos, dataBuf.end(), f, floatPrecision);
    return *this;
}

BufferedOut &BufferedOut::operator<<(ConstBytesView span)
{
    const size_t availableSize = (dataBuf.end() - pos) / 2;
    auto size = span.size() < availableSize ? span.size() : availableSize;
    for (size_t i = 0; i < size; ++i)
    {
        *this << span[i];
    }
    return *this;
}

BufferedOut &BufferedOut::operator<<(unsigned char b)
{
    char digits[] = "0123456789ABCDEF";
    *(pos++) = digits[(b & 0xF0u) >> 4];
    *(pos++) = digits[b & 0xFu];
    return *this;
}

BufferedOut &BufferedOut::operator<<(std::string_view str)
{
    auto effectiveSize = std::min(str.size(), static_cast<size_t>(dataBuf.end() - pos));
    std::memcpy(pos, str.data(), effectiveSize);
    pos += effectiveSize;
    return *this;
}

} // namespace embedded
