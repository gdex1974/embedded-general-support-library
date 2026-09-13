#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace embedded
{
// Decodes a UTF-8 byte sequence into BMP (16-bit) code points.
// Malformed sequences and non-BMP characters are skipped without error reporting.
class Utf8Decoder
{
public:
    explicit Utf8Decoder(const std::string_view text)
    : pos_(text.begin())
      , end_(text.end()) {}

    // Returns the next decoded BMP code point, or std::nullopt when the input is
    // exhausted. Invalid bytes and non-BMP sequences are skipped.
    std::optional<std::uint16_t> next();

private:
    std::optional<std::uint8_t> getNextByte()
    {
        if (pos_ != end_)
        {
            return *pos_++;
        }
        return std::nullopt;
    }

    const char* pos_;
    const char* end_;
};
} // namespace embedded
