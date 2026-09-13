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
    Size<uint8_t> size;
    uint8_t shift;
    Size<int8_t> offset;
};

// A maximal contiguous range of code points that all have glyphs. Glyphs of a
// block are stored densely, so a code point's glyph is at
//   glyph[glyphOffset + (codePoint - firstCodepoint)].
struct CodePointBlock
{
    uint16_t firstCodepoint;
    uint16_t lastCodepoint;
    uint16_t glyphOffset;
};

#pragma pack(pop)

struct FontDescriptor
{
    embedded::ConstBytesView bitmap;
    embedded::MemoryView<const GlyphDescriptor> glyph;
    embedded::MemoryView<const CodePointBlock> codePointBlocks;
};

}
