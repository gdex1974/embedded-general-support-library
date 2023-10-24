#include "FreeTypeLibrary.h"

#include "FreeTypeFace.h"

#include <freetype/ftdriver.h>
#include <freetype/ftmodapi.h>

#include <stdexcept>

FreeTypeLibrary::FreeTypeLibrary()
: ftLibrary(nullptr, FT_Done_FreeType)
{
    FT_Library ftLibraryRaw;
    if (const auto errorCode = FT_Init_FreeType(&ftLibraryRaw); errorCode != 0)
    {
        throw std::runtime_error(std::string("Failed to initialize FreeType library: ") + FT_Error_String(errorCode));
    }
    ftLibrary.reset(ftLibraryRaw);
    FT_UInt interpreter_version = TT_INTERPRETER_VERSION_35;
    FT_Property_Set(ftLibrary.get(), "truetype", "interpreter-version",
                    &interpreter_version);
}

FreeTypeFace FreeTypeLibrary::createFace(const std::string& fontPath, int faceIndex) const
{
    FT_Face ftFaceRaw;
    if (const auto errorCode = FT_New_Face(ftLibrary.get(), fontPath.c_str(), faceIndex, &ftFaceRaw); errorCode != 0)
    {
        throw std::runtime_error(std::string("Failed to create FreeType face: ") + FT_Error_String(errorCode));
    }
    return FreeTypeFace(ftFaceRaw);
}