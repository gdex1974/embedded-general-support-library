#pragma once

#include <cstdint>
#include "BaseGeometry.h"
#include "MemoryView.h"

namespace embedded::fonts
{
#pragma pack(push, 1)
struct GlyphDescriptor
{
    uint16_t bitmapOffset;
    embedded::Size<uint8_t> size;
    uint8_t shift;
    embedded::Size<int8_t> offset;
};
#pragma pack(pop)

struct FontDescriptor
{
    embedded::ConstBytesView bitmap;
    embedded::MemoryView<const GlyphDescriptor> glyph;
    uint16_t first{};
    uint16_t last{};
    uint8_t verticalShift{};
};

}
