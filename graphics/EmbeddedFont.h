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
    explicit EmbeddedFont(const FontDescriptor& descriptor) :
        fontDescriptor(descriptor) {}

    CharacterBitmap getBitmap(char symbol) const;
    Rect<int> getTextBounds(std::string_view view) const;
private:
    const FontDescriptor& fontDescriptor;
};

} // embedded
