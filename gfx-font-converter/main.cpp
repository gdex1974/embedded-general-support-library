#include "graphics/GFXFontsAdapter.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include <iostream>

#include "BitStream.h"
#include "BufferedOut.h"
#include "FontConverterConfig.h"
#include "FreeTypeFace.h"
#include "FreeTypeLibrary.h"
#include "FreeTypeGlyph.h"

using GlyphDescriptor = embedded::fonts::GlyphDescriptor;
using CodePointBlock = embedded::fonts::CodePointBlock;

namespace
{
std::string toUtf8(const std::uint32_t codePoint)
{
    if (codePoint < 0x80)
    {
        return {1, static_cast<char>(codePoint)};
    }
    if (codePoint < 0x800)
    {
        return {
            static_cast<char>(0xC0 | (codePoint >> 6)),
            static_cast<char>(0x80 | (codePoint & 0x3F))
        };
    }
    if (codePoint < 0x10000)
    {
        return {
            static_cast<char>(0xE0 | (codePoint >> 12)),
            static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)),
            static_cast<char>(0x80 | (codePoint & 0x3F))
        };
    }
    return {
        static_cast<char>(0xF0 | (codePoint >> 18)),
        static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)),
        static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)),
        static_cast<char>(0x80 | (codePoint & 0x3F))
    };
}

class BufferedStreamBase
{
protected:
    explicit BufferedStreamBase(std::ostream &outputStream)
    : bufferedOut(buffer)
      , outputStream(outputStream) {}

    ~BufferedStreamBase()
    {
        flush();
    }

    void flush()
    {
        const auto view = bufferedOut.asStringView();
        if (!view.empty())
        {
            outputStream.write(view.data(), view.end() - view.begin());
            bufferedOut.clear();
        }
    }

    embedded::BufferedOut& getBufferedOut()
    {
        return bufferedOut;
    }

private:
    std::array<char, 1024> buffer {};
    embedded::BufferedOut bufferedOut;
    std::ostream &outputStream;
};

class HexStringStream : public BufferedStreamBase
{
public:
    HexStringStream(std::ostream &outputStream,
                    const std::string &fontName,
                    const int bytesInLine = 12,
                    const int indent = 2)
    : BufferedStreamBase(outputStream)
      , bytesInLine(bytesInLine)
      , indent(indent)
    {
        getBufferedOut() << "const uint8_t " << fontName << "Bitmaps[] = {\n";
        flush();
    }

    ~HexStringStream()
    {
        getBufferedOut() << " };\n\n";
    }

    HexStringStream& operator<<(const uint8_t byte)
    {
        auto &bufferedOut = getBufferedOut();
        if (bytesWritten != 0)
        {
            bufferedOut << ',';
            if (bytesInLine == bytesWritten)
            {
                bufferedOut << '\n';
                flush();
                bytesWritten = 0;
            }
            else
            {
                bufferedOut << ' ';
            }
        }
        if (bytesWritten == 0)
        {
            for (int i = 0; i < indent; ++i)
            {
                bufferedOut << ' ';
            }
        }
        bufferedOut << "0x" << byte;
        ++bytesWritten;
        return *this;
    }

private:
    const int bytesInLine;
    const int indent;
    int bytesWritten = 0;
};

struct GlyphData
{
    uint16_t codePoint;
    GlyphDescriptor descriptor;
    std::vector<uint8_t> bitmapData;
};

// Builds the maximal contiguous code-point groups from the code points
// that were actually rendered. Input is assumed to be sorted.
class CodePointBlocksBuilder
{
public:
    void addCodePoint(const uint16_t codePoint)
    {
        if (!blocks.empty() && blocks.back().lastCodepoint + 1 == codePoint)
        {
            blocks.back().lastCodepoint = codePoint;
        }
        else
        {
            blocks.push_back({codePoint, codePoint, currentOffset});
        }
        ++currentOffset;
    }

    [[nodiscard]] auto& getBlocks() const
    {
        return blocks;
    }

private:
    std::vector<CodePointBlock> blocks;
    uint16_t currentOffset = 0;
};

class TextGlyphStream : public BufferedStreamBase
{
public:
    TextGlyphStream(std::ostream &outputStream, const std::string &fontName)
    : BufferedStreamBase(outputStream)
      , fontName(fontName)
    {
        getBufferedOut() << "const GFXglyph " << fontName << "Glyphs[] = {\n";
    }

    ~TextGlyphStream()
    {
        auto &bufferedOut = getBufferedOut();
        bufferedOut << " };";
        if (lastChar)
        {
            const auto codePoint = *lastChar;
            bufferedOut << " // 0x" << static_cast<uint8_t>(codePoint >> 8) << static_cast<uint8_t>(codePoint &
                0xFF) << " '" << toUtf8(codePoint) << "'";
        }

        bufferedOut << "\n\nconst GFXcodePointBlock " << fontName << "CodePointBlocks[] ={\n";
        const auto blocks = codePointBlocksBuilder.getBlocks();
        for (std::size_t i = 0; i < blocks.size(); ++i)
        {
            if (i > 0)
            {
                bufferedOut << ",\n";
            }
            const auto &block = blocks[i];
            bufferedOut << "  { 0x" << static_cast<uint8_t>(block.firstCodepoint >> 8)
                << static_cast<uint8_t>(block.firstCodepoint & 0xFF) << ", 0x"
                << static_cast<uint8_t>(block.lastCodepoint >> 8)
                << static_cast<uint8_t>(block.lastCodepoint & 0xFF)
                << ", " << block.glyphOffset << " }";
        }

        bufferedOut << "\n};\n\n";
        flush();
    }

    TextGlyphStream& operator<<(const GlyphData &glyphData)
    {
        const auto glyph = glyphData.descriptor;
        constexpr auto width5 = embedded::BufferedOut::width{5};
        constexpr auto width4 = embedded::BufferedOut::width{4};
        auto &bufferedOut = getBufferedOut();
        if (lastChar)
        {
            const auto codePoint = *lastChar;
            bufferedOut << ",   // 0x" << static_cast<uint8_t>(codePoint >> 8) << static_cast<uint8_t>(codePoint &
                0xFF) << " '" << toUtf8(codePoint) << "'\n";
        }
        bufferedOut << "  { " << width5 << (int)glyph.bitmapOffset << ", {" << width4 << (int)glyph.size.width
            <<
            "," << width4 << (int)glyph.size.height << " }," << width4 << (int)glyph.shift << ", { "
            << width4 <<
            (int)glyph.offset.width << "," << width5 << (int)glyph.offset.height << " } }";
        lastChar = glyphData.codePoint;
        codePointBlocksBuilder.addCodePoint(*lastChar);
        flush();
        return *this;
    }

private:
    std::string fontName;
    CodePointBlocksBuilder codePointBlocksBuilder;
    std::optional<uint16_t> lastChar;
};

class VectorStream
{
public:
    explicit VectorStream(std::vector<uint8_t> &vector)
    : vector(vector) {}

    VectorStream& operator<<(uint8_t byte)
    {
        vector.push_back(byte);
        return *this;
    }

private:
    std::vector<uint8_t> &vector;
};

void writeHeaderFile(const std::string &fontName, const std::filesystem::path &outputDir,
                     std::vector<GlyphData> &glyphsData)
{
    const auto outputFileName = (outputDir / (fontName + ".h")).string();
    std::ofstream outputFile(outputFileName, std::ios::out);
    if (!outputFile.is_open())
    {
        throw std::runtime_error("output file " + outputFileName + " was not opened");
    }
    std::cout << "Writing output to " << outputFileName << std::endl;
    {
        HexStringStream hexStringStream(outputFile, fontName);

        for (const auto &data : glyphsData)
        {
            for (const auto byte : data.bitmapData)
            {
                hexStringStream << byte;
            }
        }
    }
    {
        TextGlyphStream textGlyphStream(outputFile, fontName);
        for (const auto &glyphData : glyphsData)
        {
            textGlyphStream << glyphData;
        }
    }

    outputFile << "const GFXfont " << fontName << " = {\n"
        << "  " << fontName << "Bitmaps,\n"
        << "  " << fontName << "Glyphs,\n"
        << "  " << fontName << "CodePointBlocks\n"
        << "};\n";
}

// Replaces every character that is not a letter, digit or underscore with an
// underscore so the result is a valid C++ identifier (font file names may
// contain spaces, dashes, dots, ...).
std::string sanitizeIdentifier(const std::string &name)
{
    std::string result;
    result.reserve(name.size());
    for (const char c : name)
    {
        result += (std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_';
    }
    return result;
}

void writeBinary(const std::string &fontName, const std::filesystem::path &outputDir,
                 std::vector<GlyphData> &glyphsData)
{
    CodePointBlocksBuilder codePointBlocksBuilder;
    {
        const auto path = outputDir / (fontName + "Bitmaps.bin");
        std::ofstream out(path.string(), std::ios::out | std::ios::binary);
        if (!out.is_open())
        {
            throw std::runtime_error("output file " + path.string() + " was not opened");
        }
        std::cout << "Writing bitmaps to " << path.string() << std::endl;
        for (const auto &data : glyphsData)
        {
            out.write(reinterpret_cast<const char*>(data.bitmapData.data()),
                      static_cast<std::streamsize>(data.bitmapData.size()));
        }
    }
    {
        const auto path = outputDir / (fontName + "Glyphs.bin");
        std::ofstream out(path.string(), std::ios::out | std::ios::binary);
        if (!out.is_open())
        {
            throw std::runtime_error("output file " + path.string() + " was not opened");
        }
        std::cout << "Writing glyphs to " << path.string() << std::endl;
        for (const auto &data : glyphsData)
        {
            out.write(reinterpret_cast<const char*>(&data.descriptor), sizeof(GlyphDescriptor));
            codePointBlocksBuilder.addCodePoint(data.codePoint);
        }
    }
    {
        const auto path = outputDir / (fontName + "CodePointBlocks.bin");
        std::ofstream out(path.string(), std::ios::out | std::ios::binary);
        if (!out.is_open())
        {
            throw std::runtime_error("output file " + path.string() + " was not opened");
        }
        std::cout << "Writing code point blocks to " << path.string() << std::endl;
        for (const auto &r : codePointBlocksBuilder.getBlocks())
        {
            out.write(reinterpret_cast<const char*>(&r), static_cast<std::streamsize>(sizeof(CodePointBlock)));
        }
    }
}

std::vector<GlyphData> createTable(const std::string &fontFileName, const int dpi, const FT_F26Dot6 fontSize,
                                   const std::vector<uint16_t> &requestedCodePoints)
{
    std::vector<GlyphData> table;
    table.reserve(requestedCodePoints.size());

    FreeTypeLibrary ftLibrary;
    auto ftFace = ftLibrary.createFace(fontFileName, 0);
    ftFace.setCharSize(fontSize, 0, dpi, 0);
    {
        std::vector<uint8_t> bitmapData;
        VectorStream vectorStream(bitmapData);
        BitStream bitStream(vectorStream);
        std::uint32_t bitmapOffset = 0;
        for (const auto codePoint : requestedCodePoints)
        {
            // Skip code points the font does not contain.
            if (FT_Get_Char_Index(ftFace.get(), codePoint) == 0)
            {
                continue;
            }

            const auto glyph = ftFace.renderGlyph(codePoint, FT_LOAD_TARGET_MONO, FT_RENDER_MODE_MONO);
            const auto &[root, left, top, bitmap] = glyph.getBitmapGlyph();

            for (unsigned y = 0; y < bitmap.rows; ++y)
            {
                for (unsigned x = 0; x < bitmap.width; ++x)
                {
                    const auto byte = x / 8;
                    const uint8_t bit = 0x80 >> (x & 7);
                    bitStream << ((bitmap.buffer[y * bitmap.pitch + byte] & bit) != 0);
                }
            }
            bitStream.flush();
            const auto bitmapSize = bitmapData.size();
            table.emplace_back(GlyphData{
                codePoint, {
                    static_cast<uint16_t>(bitmapOffset), {
                        static_cast<unsigned char>(bitmap.width), static_cast<unsigned char>(bitmap.rows)
                    },
                    static_cast<uint8_t>(root.advance.x >> 16), {
                        static_cast<int8_t>(left), static_cast<int8_t>((1 - top))
                    }
                },
                std::move(bitmapData)
            });
            bitmapOffset += bitmapSize;
        }
    }
    return table;
}
} // namespace

int main(int argc, char* argv[])
{
    try
    {
        FontConverterConfig config(argc, argv);
        const auto &fontFileName = config.getFontFileName();
        if (config.getPrintHelp() || fontFileName.empty())
        {
            std::cout << "Usage: " << std::filesystem::path(argv[0]).filename().string()
                << " -f font file [-o output dir] [-d DPI] [-s size] "
                "[-u CODEPOINT|RANGE ... repeatable, e.g. -u 0x20-0x7E -u 0x4E00-0x4FFF; default 0x20-0x7E] [-b] [-h]"
                << std::endl;
            return 1;
        }

        const std::filesystem::path fontFilePath(fontFileName);
        const auto fontName = sanitizeIdentifier(fontFilePath.filename().replace_extension("").string()) +
            std::to_string(config.getFontSize()) + "pt";
        auto outputDir = std::filesystem::is_directory(config.getOutputDirectory())
                             ? std::filesystem::path(config.getOutputDirectory())
                             : std::filesystem::path(argv[0]).parent_path();

        const FT_F26Dot6 fontSize = config.getFontSize() << 6;
        std::vector<GlyphData> table = createTable(fontFileName, config.getDPI(), fontSize, config.getCodePoints());

        if (table.empty())
        {
            throw std::runtime_error("none of the requested code points have glyphs in this font");
        }

        if (config.getFormat() == FontConverterConfig::Format::Binary)
        {
            writeBinary(fontName, outputDir, table);
        }
        else
        {
            writeHeaderFile(fontName, outputDir, table);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
