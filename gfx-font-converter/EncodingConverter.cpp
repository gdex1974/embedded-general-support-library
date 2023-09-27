#include "EncodingConverter.h"

#include <sstream>

EncodingConverter::EncodingConverter(const char* from, const char* to) {
    descriptor = iconv_open(to, from);
    if (descriptor == (iconv_t) -1)
    {
        std::stringstream str;
        if (errno == EINVAL)
        {
            str << "conversion from " << from <<" to " << to << " not available";
        }
        else
        {
            str << "iconv_open filed, errno is " << errno;
        }

        throw(std::runtime_error(str.str()));
    }
}

std::string EncodingConverter::convert(char symbol) {
    buf[inSize] = symbol;
    size_t inBufSize = 1 + inSize;
    char outBuf[4] = {};
    size_t outBytesLeft = sizeof(outBuf);
    char *inptr = &buf[0];
    char *outptr = &outBuf[0];
    if(iconv(descriptor, &inptr, &inBufSize, &outptr, &outBytesLeft) == (size_t) -1)
    {
        if (errno == EINVAL)
        {
            inSize = (inSize + 1) % 3;
        }
        else
        {
            inSize = 0;
        }
        return {};
    }
    return {outBuf, sizeof(outBuf) - outBytesLeft};
}
