#include "FontConverterConfig.h"

#include <getopt.h>
#include <cctype>
#include <algorithm>

FontConverterConfig::FontConverterConfig(int argc, char** argv)
{
    int opt;
    encoding = "ASCII";
    while ((opt = getopt(argc, argv, "f:o:d:s:l:e:h")) != -1)
    {
        switch (opt)
        {
            case 'f':
                fontFileName = optarg;
                break;
            case 'o':
                outputFileName = optarg;
                break;
            case 'd':
                DPI = std::stoi(optarg);
                break;
            case 's':
                fontSize = std::stoi(optarg);
                break;
            case 'e':
                encoding = optarg;
                break;
            case 'h':
            default:
                printHelp = true;
                break;
        }
    }
    std::transform(encoding.cbegin(), encoding.cend(), encoding.begin(), std::toupper);
    if (encoding == "ASCII")
    {
        lastChar = '~';
    }
    else
    {
        lastChar = 0xFF;
    }
}
