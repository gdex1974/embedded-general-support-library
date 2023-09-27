#include "graphics/GFXFontsAdapter.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <vector>

#include "FontConverterConfig.h"
#include "BitStream.h"

#include <ft2build.h>
#include <iostream>
#include FT_GLYPH_H
#include FT_MODULE_H
#include FT_TRUETYPE_DRIVER_H
#include "BufferedOut.h"
#include "EncodingConverter.h"

using FTLibraryHandler = std::unique_ptr<FT_LibraryRec_, decltype(&FT_Done_FreeType)>;
using FTFaceHandler = std::unique_ptr<FT_FaceRec_, decltype(&FT_Done_Face)>;
using FTGlyphHandler = std::unique_ptr<FT_GlyphRec_, decltype(&FT_Done_Glyph)>;

using GlyphDescriptor = embedded::fonts::GlyphDescriptor;

class HexStringStream
{
public:
    explicit HexStringStream(embedded::BufferedOut& bufferedOut, int bytesInLine = 12, int indent = 2)
    : bufferedOut(bufferedOut), bytesInLine(bytesInLine), indent(indent) {}
    HexStringStream& operator<<(uint8_t byte)
    {
        if (bytesWritten)
        {
            bufferedOut << ", ";
            if (bytesInLine == bytesWritten)
            {
                bufferedOut << "\n";
                for (int i = 0; i < indent; ++i)
                {
                    bufferedOut << " ";
                }
                bytesWritten = 0;
            }
        }
        bufferedOut << "0x" << byte;
        ++bytesWritten;
        return *this;
    }
private:
    embedded::BufferedOut& bufferedOut;
    const int bytesInLine;
    const int indent;
    int bytesWritten = 0;
};

int main(int argc, char *argv[]) {
    try
    {
        FontConverterConfig config(argc, argv);
        const auto &fontFileName = config.getFontFileName();
        if (config.getPrintHelp() || fontFileName.empty())
        {
            std::cout << "Usage: " << std::filesystem::path(argv[0]).filename().string()
                      << " -f fontfile [-o outputfile] [-d DPI] [-s size] [-l last char] [-e 8-bit codepage, default ISO-8859-1] [-h]"
                      << std::endl;
            return 1;
        }

        auto outputFileName = config.getOutputFileName();
        const std::filesystem::path fontFilePath(fontFileName);
        const auto
                fontName =
                fontFilePath.filename().replace_extension("").string() + std::to_string(config.getFontSize()) + "pt" +
                std::to_string(config.getLastChar() > 127 ? 8 : 7) + "b";
        if (outputFileName.empty())
        {
            auto outputDir = std::filesystem::path(argv[0]).parent_path();
            outputFileName = (outputDir / (fontName + ".h")).string();
        }
        const auto dpi = config.getDPI();
        const FT_F26Dot6 fontSize = config.getFontSize() << 6;
        const auto first = config.getFirstChar();
        const auto last = config.getLastChar();

        FTLibraryHandler library(nullptr, FT_Done_FreeType);
        FTFaceHandler face(nullptr, FT_Done_Face);
        FTGlyphHandler glyph(nullptr, FT_Done_Glyph);
        std::vector<GlyphDescriptor> table;
        table.reserve(last - first + 1);

        if (FT_Library ftLibrary; FT_Init_FreeType(&ftLibrary) != 0)
        {
            return 1;
        }
        else
        {
            library.reset(ftLibrary);
        }

        FT_UInt interpreter_version = TT_INTERPRETER_VERSION_35;
        FT_Property_Set(library.get(), "truetype", "interpreter-version",
                        &interpreter_version);

        if (FT_Face ftFace; FT_New_Face(library.get(), fontFileName.c_str(), 0, &ftFace) != 0)
        {
            std::cerr << "Font load error" << std::endl;
            return 1;
        }
        else
        {
            face.reset(ftFace);
        }

        std::fstream outputFile;
        outputFile.open(outputFileName, std::ios::out);
        if (outputFile.is_open())
        {
            std::cout << "Writing output to " << outputFileName << std::endl;
        }
        else
        {
            std::cerr << "Error opening output file" << outputFileName << std::endl;
            return 1;
        }

        FT_Set_Char_Size(face.get(), fontSize, 0, dpi, 0);

        outputFile << "const uint8_t " << fontName << "Bitmaps[] = {\n  ";

        std::array<char, 1024> buffer{};
        embedded::BufferedOut bufferedOut(buffer);
        HexStringStream hexStringStream(bufferedOut);
        BitStream bitStream(hexStringStream);
        unsigned bitmapOffset = 0;
        for (auto i = first, j = 0; i <= last; i = (i == '~') ? 0xA0 : i + 1, ++j)
        {
            if (const auto err = FT_Load_Char(face.get(), i, FT_LOAD_TARGET_MONO); err != 0)
            {
                std::cerr << "Error " << err << " loading char '" << (char)i << "'" << std::endl;
                continue;
            }

            if (const auto err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO); err != 0)
            {
                std::cerr << "Error " << err << " rendering char '" << (char)i << "'" << std::endl;
                continue;
            }

            if (FT_Glyph ftGlyph; FT_Get_Glyph(face->glyph, &ftGlyph) != 0 || ftGlyph->format != FT_GLYPH_FORMAT_BITMAP)
            {
                std::cerr << "Error getting glyph '" << (char)i << "'" << std::endl;
                continue;
            }
            else
            {
                glyph.reset(ftGlyph);
            }

            const auto &bitmap = face->glyph->bitmap;
            const auto bitmapGlyphRec = reinterpret_cast<FT_BitmapGlyphRec*>(glyph.get());

            table[j].bitmapOffset = bitmapOffset;
            table[j].size.width = bitmap.width;
            table[j].size.height = bitmap.rows;
            table[j].shift = face->glyph->advance.x >> 6;
            table[j].offset.width = static_cast<decltype(table[j].offset.width)>(bitmapGlyphRec->left);
            table[j].offset.height = static_cast<decltype(table[j].offset.height)>(1 - bitmapGlyphRec->top);

            for (unsigned y = 0; y < bitmap.rows; ++y)
            {
                for (unsigned x = 0; x < bitmap.width; ++x)
                {
                    const auto byte = x / 8;
                    const uint8_t bit = 0x80 >> (x & 7);
                    bitStream << ((bitmap.buffer[y * bitmap.pitch + byte] & bit) != 0);
                }
            }

            // Pad end of char bitmap to next byte boundary if needed
            auto n = (bitmap.width * bitmap.rows) & 7;
            if (n)
            {     // Pixel count not an even multiple of 8?
                bitStream.flush();
            }
            const auto view = bufferedOut.asStringView();
            outputFile.write(view.data(), view.end() - view.begin());
            bufferedOut.clear();
            bitmapOffset += (bitmap.width * bitmap.rows + 7) / 8;
        }
        outputFile << " };\n\n"; // End bitmap array

        EncodingConverter iconvHolder(config.getEncoding().c_str(), "UTF-8");
        // Output glyph attributes table (one per character)
        outputFile << "const GFXglyph " << fontName << "Glyphs[] = {\n";
        const auto width5 = embedded::BufferedOut::width { 5 };
        const auto width4 = embedded::BufferedOut::width { 4 };
        for (auto i = first, j = 0; i <= last; i = (i == '~') ? 0xA0 : i + 1, ++j)
        {
            bufferedOut << "  { " << width5 << (int)table[j].bitmapOffset << ", {" << width4 << (int)table[j].size.width
                        <<
                        "," << width4 << (int)table[j].size.height << " }," << width4 << (int)table[j].shift << ", { "
                        << width4 <<
                        (int)table[j].offset.width << "," << width4 << (int)table[j].offset.height << " } }";
            if (i < last)
            {
                bufferedOut << ",   // 0x" << (uint8_t)i;
                if ((i >= ' ') && (i <= '~'))
                {
                    bufferedOut << " '" << (char)i << "'";
                }
                else if (i > '~')
                {
                    bufferedOut << " '" << iconvHolder.convert((char)i) << "'";
                }
                bufferedOut << '\n';
            }
            const auto view = bufferedOut.asStringView();
            outputFile.write(view.data(), view.end() - view.begin());
            bufferedOut.clear();
        }
        bufferedOut << " }; // 0x" << (uint8_t)last;
        if ((last >= ' ') && (last <= '~'))
        {
            bufferedOut << " '" << (char)last << "'";
        }
        else if (last > '~')
        {
            bufferedOut << " '" << iconvHolder.convert((char)last) << "'";
        }
        bufferedOut << "\n\n";

        // Output font structure
        bufferedOut << "const GFXfont " << fontName << " = {\n";
        bufferedOut << "  " << fontName << "Bitmaps,\n";
        bufferedOut << "  " << fontName << "Glyphs,\n";

        bufferedOut << "  0x" << (uint8_t)first << ", 0x" << (uint8_t)last << ", ";
        if (face->size->metrics.height == 0)
        {
            bufferedOut << table[0].size.height;
        }
        else
        {
            bufferedOut << (face->size->metrics.height >> 6);
        }
        bufferedOut << " };\n";

        const auto view = bufferedOut.asStringView();
        outputFile.write(view.data(), view.end() - view.begin());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}