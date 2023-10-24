#include "FreeTypeGlyph.h"

#include <stdexcept>

FT_BitmapGlyphRec &FreeTypeGlyph::getBitmapGlyph() const
{
    if (ftGlyph->format != FT_GLYPH_FORMAT_BITMAP)
    {
        throw std::runtime_error("FreeType glyph is not bitmap");
    }
    return *reinterpret_cast<FT_BitmapGlyphRec*>(ftGlyph.get());
}
