#pragma once

#include <memory>
#include <freetype/ftglyph.h>

class FreeTypeGlyph
{
public:
    explicit FreeTypeGlyph(FT_Glyph glyph) : ftGlyph(glyph, FT_Done_Glyph) {}

    FT_BitmapGlyphRec& getBitmapGlyph() const;
private:
    using FTGlyphHandler = std::unique_ptr<FT_GlyphRec_, decltype(&FT_Done_Glyph)>;
    FTGlyphHandler ftGlyph;
};
