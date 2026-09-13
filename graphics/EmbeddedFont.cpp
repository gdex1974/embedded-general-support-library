#include "EmbeddedFont.h"
#include "GFXFontsAdapter.h"
#include "Utf8Decoder.h"

#include <algorithm>
#include <cstddef>

namespace embedded::fonts
{

namespace
{

// Finds the code point block containing `codePoint`, or nullptr if none. Blocks
// are sorted ascending by firstCodepoint and are disjoint, so the answer is the
// rightmost block whose firstCodepoint is <= codePoint (if it also reaches codePoint).
const CodePointBlock *findCodePointBlock(const embedded::MemoryView<const CodePointBlock> &blocks, uint16_t codePoint)
{
    const auto *lo = blocks.begin();
    const auto *hi = blocks.end();
    while (lo < hi)
    {
        const auto mid = lo + (hi - lo) / 2;
        if (mid->firstCodepoint <= codePoint)
        {
            lo = mid + 1;
        }
        else
        {
            hi = mid;
        }
    }
    if (lo == blocks.begin())
    {
        return nullptr;
    }
    --lo;
    return (codePoint <= lo->lastCodepoint) ? lo : nullptr;
}

} // namespace

EmbeddedFont::CharacterBitmap EmbeddedFont::buildBitmap(const std::size_t index) const
{
    const auto &glyph = fontDescriptor.glyph[index];
    const auto bitmapBegin = fontDescriptor.bitmap.begin() + glyph.bitmapOffset;
    const auto bitmapEnd = (index + 1 < fontDescriptor.glyph.size())
                           ? fontDescriptor.bitmap.begin() + fontDescriptor.glyph[index + 1].bitmapOffset
                           : fontDescriptor.bitmap.end();
    return CharacterBitmap { .bitmap { bitmapBegin, static_cast<std::size_t>(bitmapEnd - bitmapBegin) },
                            .effectiveSize = glyph.size,
                            .offset = glyph.offset,
                            .width = glyph.shift };
}

EmbeddedFont::CharacterBitmap EmbeddedFont::getBitmap(uint16_t codePoint) const
{
    const auto &glyphs = fontDescriptor.glyph;
    if (glyphs.empty())
    {
        return {};
    }
    const auto *block = findCodePointBlock(fontDescriptor.codePointBlocks, codePoint);
    // Missing code points fall back to the first glyph in the table.
    const auto index = (block != nullptr) ? block->glyphOffset + (codePoint - block->firstCodepoint) : 0;
    return buildBitmap(index);
}

Rect<int> EmbeddedFont::getTextBounds(std::string_view view) const
{
    Rect<int> result{};
    if (!view.empty())
    {
        int maxBottom = -255;
        int minTop = 0;

        int count = 0;
        {
            Utf8Decoder decoder(view);
            while (decoder.next())
            {
                ++count;
            }
        }

        Utf8Decoder decoder(view);
        std::optional<uint16_t> codePoint;
        for (int i = 0; (codePoint = decoder.next()); ++i)
        {
            const auto &bitmap = getBitmap(*codePoint);
            if (i == 0)
            {
                result.topLeft.x = - bitmap.offset.width;
                result.size.width = bitmap.effectiveSize.width;
            }
            else
            {
                result.size.width += (i < count - 1) ? bitmap.width : bitmap.effectiveSize.width;
            }
            maxBottom = std::max(maxBottom, static_cast<int>(bitmap.offset.height) + bitmap.effectiveSize.height);
            minTop = std::min(minTop, static_cast<int>(bitmap.offset.height));
        }
        result.size.height = maxBottom - minTop;
        result.topLeft.y = - minTop;
    }
    return result;
}

} // embedded
