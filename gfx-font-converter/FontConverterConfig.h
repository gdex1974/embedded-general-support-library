#pragma once

#include <string>
#include <cstdint>

class FontConverterConfig
{
public:
    FontConverterConfig(int argc, char * argv[]);
    const std::string& getFontFileName() const { return fontFileName; }
    const std::string& getOutputFileName() const { return outputFileName; }
    const std::string& getEncoding() const { return encoding; }
    int getDPI() const { return DPI; }
    int getFontSize() const { return fontSize; }
    int getFirstChar() const { return firstChar; }
    int getLastChar() const { return lastChar; }
    bool getPrintHelp() const { return printHelp; }
private:
    std::string fontFileName;
    std::string outputFileName;
    std::string encoding;
    int DPI = 141;
    int fontSize = 12;
    uint8_t firstChar = ' ';
    uint8_t lastChar = 0xFF;
    bool printHelp = false;
};
