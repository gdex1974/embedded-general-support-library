#pragma once

#include "MemoryView.h"
#include <array>
#include <string_view>

namespace embedded
{

class BufferedOut
{
private:
    MemoryView<char> dataBuf;
public:
    explicit BufferedOut(MemoryView<char> dataBuf) : dataBuf(dataBuf), pos(dataBuf.begin()), floatPrecision(0) {}

    struct precision { int p; };

    BufferedOut &operator<<(const char* str);

    BufferedOut &operator<<(int n);

    BufferedOut &operator<<(unsigned int n);

    BufferedOut &operator<<(long n);

    BufferedOut &operator<<(unsigned long n);

    BufferedOut &operator<<(long long n);

    BufferedOut &operator<<(unsigned long long n);

    BufferedOut &operator<<(float f);

    BufferedOut &operator<<(ConstBytesView span);

    BufferedOut &operator<<(precision p)
    {
        floatPrecision = p.p;
        return *this;
    }

    BytesView data() { return { (unsigned char*)(dataBuf.begin()), static_cast<uint16_t>(size()) }; }
    ConstBytesView data() const { return { (unsigned char*)(dataBuf.begin()), static_cast<uint16_t>(size()) }; }

    explicit operator std::string_view() const { return { dataBuf.begin(), size() }; }

    decltype(dataBuf)::size_type size() const { return pos - dataBuf.begin(); }
    decltype(dataBuf)::size_type capacity() const { return dataBuf.size(); }

    explicit operator uint8_t*() { return (uint8_t*)(dataBuf.begin()); }
    explicit operator const uint8_t*() const { return (const uint8_t*)(dataBuf.begin()); }

    void clear()
    {
        pos = dataBuf.begin();
        floatPrecision = 0;
    }

    void addChar(char c)
    {
        if (pos != dataBuf.end())
        {
            *(pos++) = c;
        }
    }

private:
    decltype(dataBuf)::iterator pos;
    int floatPrecision;
};

}
