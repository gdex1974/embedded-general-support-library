#pragma once

#include <iconv.h>
#include <string>

class EncodingConverter
{
public:
    EncodingConverter(const char *from, const char *to);

    std::string convert(char symbol);

    ~EncodingConverter()
    {
        iconv_close(descriptor);
    }

private:
    iconv_t descriptor;
    char buf[4] = {};
    size_t inSize = 0;
};
