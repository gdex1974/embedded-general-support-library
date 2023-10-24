#include "FreeTypeFace.h"

#include "FreeTypeGlyph.h"

#include <string>
#include <stdexcept>
#include <freetype/ftglyph.h>

void FreeTypeFace::setCharSize(FT_F26Dot6 width, FT_F26Dot6 height, int horzResolution, int vertResolution) const
{
    if (const auto errorCode = FT_Set_Char_Size(ftFace.get(), width, height, horzResolution, vertResolution);
            errorCode != 0)
    {
        throw std::runtime_error(std::string("Failed to set FreeType face char size: ") + FT_Error_String(errorCode));
    }
}

FreeTypeGlyph FreeTypeFace::renderGlyph(uint32_t symbol, int loadFlags, FT_Render_Mode renderMode) const
{
    if (const auto errorCode = FT_Load_Char(ftFace.get(), symbol, loadFlags); errorCode != 0)
    {
        throw std::runtime_error(std::string("Failed to load FreeType face char code ") + std::to_string(symbol) + ": " + FT_Error_String(errorCode));
    }
    if (const auto errorCode = FT_Render_Glyph(ftFace->glyph, renderMode); errorCode != 0)
    {
        throw std::runtime_error(std::string("Failed to render FreeType face char code ") + std::to_string(symbol) + ": " + FT_Error_String(errorCode));
    }

    if (FT_Glyph ftGlyph; FT_Get_Glyph(ftFace->glyph, &ftGlyph) != 0)
    {
        throw std::runtime_error(std::string("Failed to get FreeType face glyph ") + std::to_string(symbol));
    }
    else
    {
        return FreeTypeGlyph(ftGlyph);
    }
}
