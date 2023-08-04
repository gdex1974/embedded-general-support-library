#include "PBMHelper.h"
#include <string>
#include <fstream>
#include <vector>
#include <optional>
#include <tuple>

namespace testing
{
std::optional<std::tuple<int, int, std::vector<uint8_t>>> PBMHelper::readPBM(std::string_view path)
{
    std::ifstream file(std::string(path), std::ios::binary);
    if (!file)
    {
        return std::nullopt;
    }

    std::string magicNumber;
    int width, height;
    file >> magicNumber >> width >> height;

    if (magicNumber != "P4" && magicNumber != "P1")
    {
        return std::nullopt;
    }

    // Skip the newline character after the header
    if (!file.getline(nullptr, 0).good())
    {
        return std::nullopt;
    }
    //file.ignore(1);

    // Calculate the number of bytes needed to store the image data
    int dataSize = (width * height + 7) / 8;
    if (dataSize <= 0)
    {
        return std::nullopt;
    }

    // Read the image data into a vector of bytes
    std::vector<uint8_t> imageData(dataSize);

    if (magicNumber == "P4")
    {
        file.read(reinterpret_cast<decltype(file)::char_type*>(imageData.data()),
                  static_cast<std::streamsize>(imageData.size()));
    }
    else if (magicNumber == "P1")
    {
        // Read the ASCII-encoded image data
        std::string asciiData;
        int dataIndex = 0;
        while (std::getline(file, asciiData))
        {
            if (asciiData.empty() || asciiData[0] == '#')
            {
                continue;
            }

            // Convert the ASCII-encoded data to binary
            for (char c: asciiData)
            {
                if (c == '0' || c == '1')
                {
                    imageData[dataIndex / 8] |= (c - '0') << (7 - (dataIndex % 8));
                    dataIndex++;
                }
                if (dataIndex >= dataSize * 8)
                {
                    break;
                }
            }
        }
    }

    return std::make_tuple(width, height, imageData);
}

bool PBMHelper::writePBM(const std::string &path, embedded::ConstBytesView data, int width, int height)
{
    std::ofstream file(path);
    if (!file)
    {
        return false; // Return false if file cannot be opened
    }

    // Write the PBM file header
    file << "P1\n";
    file << width << " " << height << "\n";

    // Write the image data
    int dataIndex = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < (width + 7) / 8; ++x)
        {
            if (dataIndex >= data.size())
            {
                return false;
            }
            uint8_t byte = data[dataIndex++];
            for (int bit = 7; bit >= 0; --bit)
            {
                file << (((byte >> bit) & 1) ? '1' : '0') << " ";
            }
        }
        file << "\n";
    }

    return true;
}

bool PBMHelper::writePBMBinary(const std::string &path, embedded::ConstBytesView data, int width, int height)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        return false;
    }

    // Write the PBM file header
    file << "P4\n";
    file << width << " " << height << "\n";

    // Calculate the number of bytes needed to store the image data
    int dataSize = std::min((width * height + 7) / 8, static_cast<int>(data.size()));

    // Write the image data
    file.write(reinterpret_cast<const char*>(data.begin()), dataSize);

    return true;
}

bool PBMHelper::comparePBM(const std::string &path, embedded::ConstBytesView data, int width, int height)
{
    auto fileData = readPBM(path);
    if (!fileData)
    {
        return false;
    }

    if (std::get<2>(*fileData).size() != data.size() || std::get<0>(*fileData) != width ||
        std::get<1>(*fileData) != height)
    {
        return false;
    }

    const auto &readedData = std::get<2>(*fileData);
    for (int i = 0; i < readedData.size(); ++i)
    {
        if (readedData[i] != data[i])
        {
            return false;
        }
    }

    return true;
}

} // testing