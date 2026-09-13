#pragma once

#include "MemoryView.h"
#include "BaseGeometry.h"

namespace embedded::fonts
{
struct FontDescriptor;

class EmbeddedFont
{
public:
    struct CharacterBitmap
    {
        embedded::ConstBytesView bitmap;
        embedded::Size<uint8_t> effectiveSize;
        embedded::Size<int8_t> offset;
        uint8_t width;
    };

    explicit EmbeddedFont(const FontDescriptor &descriptor) noexcept :
        fontDescriptor(descriptor) {}

    // Returns the glyph for a BMP code point. Code points the font does not
    // contain fall back to the first glyph in the table.
    CharacterBitmap getBitmap(uint16_t codePoint) const;
    Rect<int> getTextBounds(std::string_view view) const;

private:
    CharacterBitmap buildBitmap(std::size_t index) const;
    const FontDescriptor &fontDescriptor;
};

} // embedded
