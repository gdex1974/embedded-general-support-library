#include <gtest/gtest.h>
#include "EndianConversion.h"

using namespace embedded;

TEST(EndianConversionTest, UInt16Conversion)
{
    EXPECT_EQ(changeEndianess(static_cast<uint16_t>(0xABCD)), static_cast<uint16_t>(0xCDAB));
    EXPECT_EQ(changeEndianess(static_cast<uint16_t>(0x1234)), static_cast<uint16_t>(0x3412));
}

TEST(EndianConversionTest, UInt32Conversion)
{
    EXPECT_EQ(changeEndianess(static_cast<uint32_t>(0xABCDEF12)), static_cast<uint32_t>(0x12EFCDAB));
    EXPECT_EQ(changeEndianess(static_cast<uint32_t>(0x12345678)), static_cast<uint32_t>(0x78563412));
}

TEST(EndianConversionTest, FloatConversion)
{
    union {
        float f;
        uint32_t u;
    } converter { 5.677587e-19 };
    uint32_t swapped = changeEndianess(converter.u);
    EXPECT_FLOAT_EQ(changeEndianessToFloat(swapped), converter.f);
    converter.f = -1.763874e+36;
    swapped = changeEndianess(converter.u);
    EXPECT_FLOAT_EQ(changeEndianessToFloat(swapped), converter.f);
}

TEST(EndianConversionTest, 16x2_Conversion)
{
    uint32_t value = 0xABCDEF12;
    changeEndianess16x2(&value);
    EXPECT_EQ(value, static_cast<uint32_t>(0xCDAB12EF));

    value = 0x12345678;
    changeEndianess16x2(&value);
    EXPECT_EQ(value, static_cast<uint32_t>(0x34127856));
}
