#pragma once

#include "BufferedOut.h"

namespace embedded
{
class PacketUart;

class SerialOut
{
public:
    struct end {};
    struct endl {};
    using precision = embedded::BufferedOut::precision;

    SerialOut(MemoryView<char> buffer, PacketUart &uart) : uartDevice(uart), bufferedOut(buffer) {}

    SerialOut &operator<<(const char* str)
    {
        bufferedOut << str;
        return *this;
    }

    SerialOut &operator<<(int n)
    {
        bufferedOut << n;
        return *this;
    }

    SerialOut &operator<<(unsigned int n)
    {
        bufferedOut << n;
        return *this;
    }

    SerialOut &operator<<(long n)
    {
        bufferedOut << n;
        return *this;
    }

    SerialOut &operator<<(unsigned long n)
    {
        bufferedOut << n;
        return *this;
    }

    SerialOut &operator<<(long long n)
    {
        bufferedOut << n;
        return *this;
    }

    SerialOut &operator<<(unsigned long long n)
    {
        bufferedOut << n;
        return *this;
    }

    SerialOut &operator<<(float f)
    {
        bufferedOut << f;
        return *this;
    }

    SerialOut &operator<<(precision p)
    {
        bufferedOut << p;
        return *this;
    }

    SerialOut &operator<<(embedded::BytesView span)
    {
        bufferedOut << span;
        return *this;
    }

    SerialOut &operator<<(end)
    {
        flush();
        return *this;
    }

    SerialOut &operator<<(endl)
    {
        if (bufferedOut.size() == bufferedOut.capacity())
        {
            flush();
        }
        bufferedOut.addChar('\n');
        flush();
        return *this;
    }

private:
    void flush();

    PacketUart &uartDevice;
    BufferedOut bufferedOut;
};

}
