#include "graphics/GFXFontsAdapter.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <vector>
#include <optional>

#include "FontConverterConfig.h"
#include "BitStream.h"

#include <iostream>
#include "BufferedOut.h"
#include "EncodingConverter.h"
#include "FreeTypeFace.h"
#include "FreeTypeLibrary.h"
#include "FreeTypeGlyph.h"

using GlyphDescriptor = embedded::fonts::GlyphDescriptor;

class BufferedStreamBase
{
protected:
    explicit BufferedStreamBase(std::ostream &outputStream)
            : bufferedOut(buffer)
              , outputStream(outputStream)
    {
    }

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

    embedded::BufferedOut &getBufferedOut() { return bufferedOut; }

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
                    int bytesInLine = 12,
                    int indent = 2)
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

    HexStringStream &operator<<(uint8_t byte)
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
        ++totalBytesWritten;
        return *this;
    }

    int getBytesWritten() const { return totalBytesWritten; }

private:
    const int bytesInLine;
    const int indent;
    int bytesWritten = 0;
    int totalBytesWritten = 0;
};

class TextGlyphStream : public BufferedStreamBase
{
public:
    TextGlyphStream(std::ostream &outputStream, const std::string &fontName, const std::string &encoding)
            : BufferedStreamBase(outputStream)
              , fontName(fontName)
              , encoding(encoding)
              , lastChar(encoding == "ASCII" ? '~' : 0xFF)
    {
        iconvHolder.emplace(encoding.c_str(), "UTF-8");
        outputStream << "const GFXglyph " << fontName << "Glyphs[] = {\n";
    }

    ~TextGlyphStream()
    {
        auto &bufferedOut = getBufferedOut();
        bufferedOut << "\n";
        flush();
    }

    TextGlyphStream &operator<<(const GlyphDescriptor &glyph)
    {
        constexpr auto width5 = embedded::BufferedOut::width { 5 };
        constexpr auto width4 = embedded::BufferedOut::width { 4 };
        auto &bufferedOut = getBufferedOut();
        bufferedOut << "  { " << width5 << (int)bitmapOffset << ", {" << width4 << (int)glyph.size.width
                    <<
                    "," << width4 << (int)glyph.size.height << " }," << width4 << (int)glyph.shift << ", { "
                    << width4 <<
                    (int)glyph.offset.width << "," << width5 << (int)glyph.offset.height << " } }";
        bitmapOffset += glyph.bitmapOffset;
        if (currentGlyph < lastChar)
        {
            bufferedOut << ",   // 0x" << (uint8_t)currentGlyph;
        }
        else
        {
            bufferedOut << " }; // 0x" << (uint8_t)currentGlyph;
        }
        if ((currentGlyph >= ' ') && (currentGlyph <= '~'))
        {
            bufferedOut << " '" << (char)currentGlyph << "'";
        }
        else if (currentGlyph > '~')
        {
            bufferedOut << " '" << iconvHolder->convert((char)currentGlyph) << "'";
        }
        bufferedOut << '\n';

        if (currentGlyph < lastChar)
        {
            if (currentGlyph == '~')
            {
                currentGlyph = 0xA0;
            }
            else
            {
                ++currentGlyph;
            }
        }
        flush();
        return *this;
    }

private:
    std::optional<EncodingConverter> iconvHolder;
    std::string fontName;
    std::string encoding;
    int currentGlyph = 0x20;
    const int lastChar;
    size_t bitmapOffset = 0;
};

struct GlyphData
{
    GlyphDescriptor descriptor;
    std::vector<uint8_t> bitmapData;
};

class VectorStream
{
public:
    explicit VectorStream(std::vector<uint8_t> &vector)
            : vector(vector)
    {
    }

    VectorStream &operator<<(uint8_t byte)
    {
        vector.push_back(byte);
        return *this;
    }

private:
    std::vector<uint8_t> &vector;
};

void writeHeaderFile(const std::string &fontName, const std::filesystem::path &outputDir
                     , const std::string &encoding, const std::vector<GlyphData> &glyphData
                     , int first, int last, int sizeMetricsHeight)
{
    std::ofstream outputFile;
    const auto outputFileName = (outputDir / (fontName + ".h")).string();
    outputFile.open(outputFileName, std::ios::out);
    if (!outputFile.is_open())
    {
        throw std::runtime_error("output file " + outputFileName + " was not opened");
    }
    std::cout << "Writing output to " << outputFileName << std::endl;
    {
        HexStringStream hexStringStream(outputFile, fontName);

        for (const auto &glyph: glyphData)
        {
            for (const auto &byte: glyph.bitmapData)
            {
                hexStringStream << byte;
            }
        }
    }
    {
        TextGlyphStream textGlyphStream(outputFile, fontName, encoding);
        for (const auto &glyph: glyphData)
        {
            textGlyphStream << glyph.descriptor;
        }
    }

    std::array<char, 1024> buffer {};
    embedded::BufferedOut bufferedOut(buffer);
    // Output font structure
    bufferedOut << "const GFXfont " << fontName << " = {\n";
    bufferedOut << "  " << fontName << "Bitmaps,\n";
    bufferedOut << "  " << fontName << "Glyphs,\n";

    bufferedOut << "  0x" << (uint8_t)first << ", 0x" << (uint8_t)last << ", ";
    bufferedOut << sizeMetricsHeight;
    bufferedOut << " };\n";

    const auto view = bufferedOut.asStringView();
    outputFile.write(view.data(), view.end() - view.begin());
}

int main(int argc, char* argv[])
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

    try
    {
        const std::filesystem::path fontFilePath(fontFileName);
        const auto
                fontName =
                fontFilePath.filename().replace_extension("").string() + std::to_string(config.getFontSize()) + "pt" +
                std::to_string(config.getLastChar() > 127 ? 8 : 7) + "b";
        auto outputDir = std::filesystem::path(argv[0]).parent_path();

        const auto dpi = config.getDPI();
        const FT_F26Dot6 fontSize = config.getFontSize() << 6;
        const auto first = config.getFirstChar();
        const auto last = config.getLastChar();
        std::vector<GlyphData> table;
        table.reserve(last - first + 1);

        FreeTypeLibrary ftLibrary;
        auto ftFace = ftLibrary.createFace(fontFileName, 0);
        ftFace.setCharSize(fontSize, 0, dpi, 0);
        {
            std::vector<uint8_t> bitmapData;
            VectorStream vectorStream(bitmapData);
            BitStream bitStream(vectorStream);
            EncodingConverter iconvHolder(config.getEncoding().c_str(), "UCS-4LE");
            for (auto i = first, j = 0; i <= last; i = (i == '~') ? 0xA0 : i + 1, ++j)
            {
                auto glyph = ftFace.renderGlyph(iconvHolder.convertRaw(i), FT_LOAD_TARGET_MONO, FT_RENDER_MODE_MONO);

                const auto &bitmapGlyphRec = glyph.getBitmapGlyph();
                const auto &bitmap = bitmapGlyphRec.bitmap;

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
                table.emplace_back(GlyphData { { static_cast<uint16_t>(bitmapSize), {
                        static_cast<unsigned char>(bitmap.width), static_cast<unsigned char>(bitmap.rows) }
                                            , static_cast<uint8_t>(bitmapGlyphRec.root.advance.x >> 16), {
                        static_cast<char>(bitmapGlyphRec.left), static_cast<char>((1 - bitmapGlyphRec.top)) } }
                                            , std::move(bitmapData) });

            }
        }
        if (config.getFormat() == FontConverterConfig::Format::Binary)
        {
            {
                std::ofstream outputFile;
                const auto bitmapsFileName = (outputDir / (fontName + "Bitmaps.bin")).string();
                outputFile.open(bitmapsFileName, std::ios::out | std::ios::binary);
                if (!outputFile.is_open())
                {
                    throw std::runtime_error("output file " + bitmapsFileName + " was not opened");
                }
                std::cout << "Writing bitmaps to " << bitmapsFileName << std::endl;
                for (const auto &glyph: table)
                {
                    outputFile.write(reinterpret_cast<const char*>(glyph.bitmapData.data()), glyph.bitmapData.size());
                }
            }
            {
                std::ofstream outputFile;
                const auto glyphsFileName = (outputDir / (fontName + "Glyphs.bin")).string();
                outputFile.open(glyphsFileName, std::ios::out | std::ios::binary);
                if (!outputFile.is_open())
                {
                    throw std::runtime_error("output file " + glyphsFileName + " was not opened");
                }
                std::cout << "Writing glyphs to " << glyphsFileName << std::endl;
                uint16_t offset=0;
                for (const auto &glyph: table)
                {
                    union
                    {
                        GlyphDescriptor glyphDescriptor;
                        std::array<char, sizeof(GlyphDescriptor)> bytes;
                    } descriptor;
                    descriptor.glyphDescriptor = glyph.descriptor;
                    descriptor.glyphDescriptor.bitmapOffset = offset;
                    offset += glyph.bitmapData.size();
                    outputFile.write(descriptor.bytes.cbegin(), descriptor.bytes.size());
                }
            }
        }
        else if (config.getFormat() == FontConverterConfig::Format::Header)
        {
            writeHeaderFile(fontName, outputDir, config.getEncoding(), table, first, last,
                            ftFace.getSizeMetricHeihgt());
        }
        else
        {
            throw std::runtime_error("Unknown format");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}