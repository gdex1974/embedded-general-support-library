#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class FontConverterConfig
{
public:
    enum class Format { Header, Binary};
    FontConverterConfig(int argc, char * argv[]);
    const std::string& getFontFileName() const { return fontFileName; }
    const std::string& getOutputDirectory() const { return outputDirectory; }
    // The requested code points, expanded from ranges, sorted and de-duplicated.
    const std::vector<uint16_t>& getCodePoints() const { return codePoints; }
    int getDPI() const { return DPI; }
    int getFontSize() const { return fontSize; }
    Format getFormat() const { return format; }
    bool getPrintHelp() const { return printHelp; }

private:
    // Parses a single -u value: either one code point ("0xH") or one range
    // ("first-last"). The option may be repeated; results are combined, sorted
    // and de-duplicated in the constructor.
    void parseCodePointSpec(std::string_view spec);
    void appendRange(uint16_t first, uint16_t last);

    std::string fontFileName;
    std::string outputDirectory;
    std::vector<uint16_t> codePoints;
    int DPI = 141;
    int fontSize = 12;
    Format format = Format::Header;
    bool printHelp = false;
};
