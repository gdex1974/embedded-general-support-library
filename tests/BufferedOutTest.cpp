#include "BufferedOut.h"
#include "gtest/gtest.h"

namespace
{
    std::array<char, 128> buffer;
}

using embedded::BufferedOut;

TEST(BufferedOutTest, DefaultConstructor)
{
    BufferedOut out(buffer);
    EXPECT_EQ(out.size(), 0);
    EXPECT_EQ(out.capacity(), 128);
}

TEST(BufferedOutTest, OperatorInt)
{
    BufferedOut out(buffer);
    out << 123;
    EXPECT_EQ(static_cast<std::string_view>(out), "123");
}

TEST(BufferedOutTest, OperatorFloat)
{
    BufferedOut out(buffer);
    out << BufferedOut::precision { 2 } << 123.456f;
    EXPECT_EQ(static_cast<std::string_view>(out), "123.45");
}

TEST(BufferedOutTest, OperatorBytesView)
{
    BufferedOut out(buffer);
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    out << embedded::BytesView { data, sizeof(data) };
    embedded::BytesView result = out.data();
    ASSERT_EQ(result.size(), 6);
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(result.begin()), result.size()), "010203");
}

TEST(BufferedOutTest, Clear)
{
    BufferedOut out(buffer);
    out << 123;
    out.clear();
    EXPECT_EQ(out.size(), 0);
}
