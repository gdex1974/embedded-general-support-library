#pragma once
#include <memory>
#include <string>
#include <freetype/freetype.h>


class FreeTypeFace;

class FreeTypeLibrary
{
public:
    FreeTypeLibrary();
    FT_Library get() const { return ftLibrary.get(); }
    FreeTypeFace createFace(const std::string& fontPath, int faceIndex = 0) const;
private:
    using FTLibraryHandler = std::unique_ptr<FT_LibraryRec_, decltype(&FT_Done_FreeType)>;
    FTLibraryHandler ftLibrary;
};
