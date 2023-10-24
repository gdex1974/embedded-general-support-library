#pragma once

#include <iconv.h>
#include <string>
#include <cstdint>

class EncodingConverter
{
public:
    EncodingConverter(const char *from, const char *to);

    std::string convert(char symbol);
    uint32_t convertRaw(char symbol);

    ~EncodingConverter()
    {
        iconv_close(descriptor);
    }

private:
    iconv_t descriptor;
    char buf[4] = {};
    size_t inSize = 0;
};
