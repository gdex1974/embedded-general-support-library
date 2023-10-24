#include "EmbeddedFont.h"
#include "GFXFontsAdapter.h"

namespace embedded::fonts
{

EmbeddedFont::CharacterBitmap EmbeddedFont::getBitmap(const char symbol) const
{
    if (symbol < fontDescriptor.first || symbol > fontDescriptor.last)
    {
        return getBitmap(fontDescriptor.first);
    }
    auto pos = symbol - fontDescriptor.first;
    if (symbol > '~')
    {
        pos -= (0xA0 - '~' - 1);
    }

    if (pos >= fontDescriptor.glyph.size())
    {
        return getBitmap(fontDescriptor.first);
    }
    const auto &glyph = fontDescriptor.glyph[pos];
    const auto bitmapBegin = fontDescriptor.bitmap.begin() + glyph.bitmapOffset;
    ++pos;
    const auto bitmapEnd = (pos != fontDescriptor.glyph.size()) ?
                           fontDescriptor.bitmap.end() :
                           fontDescriptor.bitmap.begin() + fontDescriptor.glyph[pos].bitmapOffset;
    return EmbeddedFont::CharacterBitmap { .bitmap { bitmapBegin, static_cast<uint16_t>(bitmapEnd - bitmapBegin) },
                                           .effectiveSize = glyph.size,
                                           .offset = glyph.offset,
                                           .width = glyph.shift };
}

Rect<int> EmbeddedFont::getTextBounds(std::string_view view) const
{
    Rect<int> result{};
    if (view.length() > 0)
    {
        int maxBottom = -255;
        int minTop = 0;
        for (std::size_t i = 0; i < view.size(); ++i)
        {
            const auto& bitmap = getBitmap(view[i]);
            if (i == 0)
            {
                result.topLeft.x = - bitmap.offset.width;
                result.size.width = bitmap.effectiveSize.width;
            }
            else
            {
                result.size.width += (i < view.size() - 1) ? bitmap.width : bitmap.effectiveSize.width;
            }
            maxBottom = std::max(maxBottom, (int)bitmap.offset.height + bitmap.effectiveSize.height);
            minTop = std::min(minTop, (int)bitmap.offset.height);
        }
        result.size.height = maxBottom - minTop;
        result.topLeft.y = - minTop;
    }
    return result;
}

} // embedded