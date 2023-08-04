#pragma once

#include <optional>
#include <vector>
#include <cstdint>
#include <string>
#include "MemoryView.h"

namespace testing
{

class PBMHelper
{
public:
    static std::optional<std::tuple<int, int, std::vector<uint8_t>>> readPBM(std::string_view path);
    static bool writePBM(const std::string& path, embedded::ConstBytesView data, int width, int height);
    static bool writePBMBinary(const std::string& path, embedded::ConstBytesView data, int width, int height);
    static bool comparePBM(const std::string& path, embedded::ConstBytesView data, int width, int height);
private:

};

} // testing
