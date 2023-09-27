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
    EXPECT_EQ(out.asStringView(), "123");
}

TEST(BufferedOutTest, OperatorLongLong) {
    BufferedOut out(buffer);
    out << 123456789012345LL;
    EXPECT_EQ(out.asStringView(), "123456789012345");
}

TEST(BufferedOutTest, OperatorUnsignedLongLong) {
    BufferedOut out(buffer);
    out << 923456789012345ULL;
    EXPECT_EQ(out.asStringView(), "923456789012345");
}

TEST(BufferedOutTest, OperatorFloat)
{
    BufferedOut out(buffer);
    out << BufferedOut::precision { 2 } << 123.456f;
    EXPECT_EQ(out.asStringView(), "123.45");
}

TEST(BufferedOutTest, OperatorBytesView)
{
    BufferedOut out(buffer);
    uint8_t data[] = { 0x01, 0x02, 0xF3 };
    out << embedded::BytesView { data };
    embedded::BytesView result = out.data();
    ASSERT_EQ(result.size(), 6);
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(result.begin()), result.size()), "0102F3");
}

TEST(BufferedOutTest, OperatorConstBytesView) {
    BufferedOut out(buffer);
    uint8_t data[] = { 0x01, 0x02, 0xF3 };
    out << embedded::BytesView { data };
    embedded::BytesView result = out.data();
    ASSERT_EQ(result.size(), 6);
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(result.begin()), result.size()), "0102F3");
}

TEST(BufferedOutTest, Width) {
    BufferedOut out(buffer);
    out << BufferedOut::width{6} << 42 << 123;
    EXPECT_EQ(std::string(out.data().begin(), out.data().end()), "    42123");
}

TEST(BufferedOutTest, FillWidth) {
    BufferedOut out(buffer);
    out << BufferedOut::fill{'0'} << BufferedOut::width{6} << 42;
    EXPECT_EQ(std::string(out.data().begin(), out.data().end()), "000042");
}

TEST(BufferedOutTest, Size) {
    BufferedOut out(buffer);
    out << "Hello";
    EXPECT_EQ(out.size(), 5);
}

TEST(BufferedOutTest, Capacity) {
    BufferedOut out(buffer);
    EXPECT_EQ(out.capacity(), sizeof(buffer));
}

TEST(BufferedOutTest, Clear)
{
    BufferedOut out(buffer);
    out << 123;
    out.clear();
    EXPECT_EQ(out.size(), 0);
}
