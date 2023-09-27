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
    explicit BufferedOut(MemoryView<char> dataBuf)
    : dataBuf(dataBuf)
    {
        clear();
    }

    struct precision { int p; };
    struct width { int w; };
    struct fill { char f; };

    BufferedOut &operator<<(const char* str);
    BufferedOut &operator<<(std::string_view str);
    BufferedOut &operator<<(int n);
    BufferedOut &operator<<(unsigned int n);
    BufferedOut &operator<<(long n);
    BufferedOut &operator<<(unsigned long n);
    BufferedOut &operator<<(long long n);
    BufferedOut &operator<<(unsigned long long n);
    BufferedOut &operator<<(float f);
    BufferedOut &operator<<(unsigned char b);
    BufferedOut &operator<<(ConstBytesView span);

    BufferedOut &operator<<(char c)
    {
        addChar(c);
        return *this;
    }

    BufferedOut &operator<<(precision p)
    {
        floatPrecision = p.p;
        return *this;
    }

    BufferedOut &operator<<(width w)
    {
        integerWidth = w.w;
        return *this;
    }

    BufferedOut &operator<<(fill f)
    {
        fillChar = f.f;
        return *this;
    }

    BytesView data() { return { (unsigned char*)(dataBuf.begin()), static_cast<uint16_t>(size()) }; }
    ConstBytesView data() const { return { (unsigned char*)(dataBuf.begin()), static_cast<uint16_t>(size()) }; }
    std::string_view asStringView() const { return { dataBuf.begin(), size() }; }

    decltype(dataBuf)::size_type size() const { return pos - dataBuf.begin(); }
    decltype(dataBuf)::size_type capacity() const { return dataBuf.size(); }

    explicit operator uint8_t*() { return (uint8_t*)(dataBuf.begin()); }
    explicit operator const uint8_t*() const { return (const uint8_t*)(dataBuf.begin()); }

    void clear()
    {
        pos = dataBuf.begin();
        floatPrecision = 0;
        integerWidth = 0;
        fillChar = ' ';
    }

    void addChar(char c)
    {
        if (pos != dataBuf.end())
        {
            *(pos++) = c;
        }
    }

private:
    void addPadding(const char* p);

    decltype(dataBuf)::iterator pos;
    int floatPrecision;
    int integerWidth;
    char fillChar;
    void adjustPos(char* p);
};

}
