#pragma once

#include <string>
#include <cstdint>

class FontConverterConfig
{
public:
    enum class Format { Header, Binary};
    FontConverterConfig(int argc, char * argv[]);
    const std::string& getFontFileName() const { return fontFileName; }
    const std::string& getOutputFileName() const { return outputFileName; }
    const std::string& getEncoding() const { return encoding; }
    int getDPI() const { return DPI; }
    int getFontSize() const { return fontSize; }
    int getFirstChar() const { return firstChar; }
    int getLastChar() const { return lastChar; }
    Format getFormat() const { return format; }
    bool getPrintHelp() const { return printHelp; }
private:
    std::string fontFileName;
    std::string outputFileName;
    std::string encoding;
    int DPI = 141;
    int fontSize = 12;
    uint8_t firstChar = ' ';
    uint8_t lastChar = 0xFF;
    Format format = Format::Header;
    bool printHelp = false;
};
