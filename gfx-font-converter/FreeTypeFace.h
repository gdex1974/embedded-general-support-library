#pragma once

#include <memory>
#include <freetype/freetype.h>

class FreeTypeGlyph;

class FreeTypeFace
{
public:
    explicit FreeTypeFace(FT_Face face) : ftFace(face, FT_Done_Face) {}
    FT_Face get() const { return ftFace.get(); }
    void setCharSize(FT_F26Dot6 width, FT_F26Dot6 height, int horzResolution, int vertResolution) const;
    FreeTypeGlyph renderGlyph(uint32_t symbol, int loadFlags, FT_Render_Mode renderMode) const;
    double getSizeMetricHeihgt() const { return ftFace->size->metrics.height/64.; }
private:
    using FTFaceHandler = std::unique_ptr<std::remove_pointer_t<FT_Face>, decltype(&FT_Done_Face)>;
    FTFaceHandler ftFace;
};
