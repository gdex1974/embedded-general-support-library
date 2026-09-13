#include "FontConverterConfig.h"

#include <getopt.h>
#include <cctype>
#include <algorithm>
#include <stdexcept>

namespace
{
uint16_t parseHex(const std::string &text)
{
    std::size_t parsed = 0;
    const auto value = std::stoul(text, &parsed, 0);
    // Reject trailing garbage (e.g. a leftover comma from the old
    // comma-separated form) instead of silently parsing only the prefix.
    if (parsed != text.size())
    {
        throw std::runtime_error("invalid code point '" + text + "'");
    }
    if (value > 0xFFFF)
    {
        throw std::runtime_error("code point " + text + " is outside the BMP (0x0000-0xFFFF)");
    }
    return static_cast<uint16_t>(value);
}

std::string trim(std::string_view text)
{
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())))
    {
        text.remove_prefix(1);
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())))
    {
        text.remove_suffix(1);
    }
    return std::string(text);
}
} // namespace

FontConverterConfig::FontConverterConfig(int argc, char** argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "f:o:d:s:u:bh")) != -1)
    {
        switch (opt)
        {
            case 'f':
                fontFileName = optarg;
                break;
            case 'o':
                outputDirectory = optarg;
                break;
            case 'd':
                DPI = std::stoi(optarg);
                break;
            case 's':
                fontSize = std::stoi(optarg);
                break;
            case 'u':
                parseCodePointSpec(optarg);
                break;
            case 'b':
                format = Format::Binary;
                break;
            case 'h':
            default:
                printHelp = true;
                break;
        }
    }
    // Sort and de-duplicate everything collected from the (possibly repeated)
    // -u options.
    std::sort(codePoints.begin(), codePoints.end());
    codePoints.erase(std::unique(codePoints.begin(), codePoints.end()), codePoints.end());
    if (codePoints.empty())
    {
        appendRange(0x20, 0x7E);
    }
}

void FontConverterConfig::appendRange(uint16_t first, uint16_t last)
{
    if (first > last)
    {
        std::swap(first, last);
    }
    codePoints.reserve(codePoints.size() + (last - first + 1));
    for (long codePoint = first; codePoint <= last; ++codePoint)
    {
        codePoints.push_back(static_cast<uint16_t>(codePoint));
    }
}

void FontConverterConfig::parseCodePointSpec(std::string_view spec)
{
    // Each -u option is a single code point or a "first-last" range.
    const auto token = trim(spec);
    if (token.empty())
    {
        return;
    }
    const std::size_t dash = token.find('-');
    if (dash == std::string::npos)
    {
        const auto value = parseHex(token);
        appendRange(value, value);
    }
    else
    {
        appendRange(parseHex(token.substr(0, dash)), parseHex(token.substr(dash + 1)));
    }
}
