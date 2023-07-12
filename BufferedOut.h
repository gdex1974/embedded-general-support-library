#pragma once

#include <array>
#include <string_view>
#include "MemoryView.h"

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

    BufferedOut &operator<<(BytesView span);

    BufferedOut &operator<<(precision p)
    {
        floatPrecision = p.p;
        return *this;
    }

    BytesView data() { return { (unsigned char*)(dataBuf.begin()), static_cast<uint16_t>(size()) }; }

    explicit operator std::string_view() { return { dataBuf.begin(), size() }; }

    decltype(dataBuf)::size_type size() { return pos - dataBuf.begin(); }

    decltype(dataBuf)::size_type capacity() { return dataBuf.size(); }

    explicit operator uint8_t*() { return (uint8_t*)(dataBuf.begin()); }

    void clear() { pos = dataBuf.begin(); }

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
