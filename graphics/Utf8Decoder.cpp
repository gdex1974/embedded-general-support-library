#include "Utf8Decoder.h"

std::optional<uint16_t> embedded::Utf8Decoder::next()
{
    std::optional<uint8_t> b0;
    while ((b0 = getNextByte()))
    {
        if (*b0 < 0x80) // 1 byte: 0xxxxxxx
        {
            return *b0;
        }

        uint32_t result = 0;
        int continuationBytes = 0;
        uint32_t firstCodePoint = 0;
        uint32_t lastCodePoint = 0;
        if ((*b0 & 0xF8) == 0xF0) // 4 bytes sequence: 11110uvv 10vvwwww 10xxxxyy 10yyzzzz
        {
            result = *b0 & 0x07;
            continuationBytes = 3;
            firstCodePoint = 0x10000;
            lastCodePoint = 0x10FFFF;
        }
        else if ((*b0 & 0xF0) == 0xE0) // 3 bytes sequence: 1110wwww 10xxxxyy 10yyzzzz
        {
            result = *b0 & 0x0F;
            continuationBytes = 2;
            firstCodePoint = 0x800;
            lastCodePoint = 0xFFFF;
        }
        else if ((*b0 & 0xE0) == 0xC0) // 2 bytes sequence: 1100xxxyy 10yyzzzz
        {
            result = *b0 & 0x1F;
            continuationBytes = 1;
            firstCodePoint = 0x80;
            lastCodePoint = 0x7FF;
        }
        else // Ignore wrong start byte
        {
            continue;
        }

        while (pos_ != end_ && continuationBytes > 0)
        {
            if (!((*pos_ & 0xC0) == 0x80))
            {
                break;
            }
            result = (result << 6) + (*getNextByte() & 0x3F);
            --continuationBytes;
        }

        // Ignore malformed sequences, non-BPM code points and surrogates (U+D800 - U+DFFF)
        if (continuationBytes == 0
            && result >= firstCodePoint
            && result <= std::min(lastCodePoint, static_cast<uint32_t>(0xFFFFu))
            && (result < 0xD800 || result > 0xDFFF))
        {
            return result;
        }
    }
    return std::nullopt;
}
