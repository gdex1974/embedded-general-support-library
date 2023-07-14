#include <gtest/gtest.h>
#include "MemoryView.h"

using embedded::MemoryView;

TEST(MemoryViewTest, DefaultConstructor)
{
MemoryView<char> view;
EXPECT_EQ(view.size(), 0);
EXPECT_EQ(view.begin(), nullptr);
EXPECT_EQ(view.end(), nullptr);
}

TEST(MemoryViewTest, Constructor)
{
char data[] = { 0x01, 0x02, 0x03 };
MemoryView view { data, sizeof(data) };
EXPECT_EQ(view.size(), 3);
EXPECT_EQ(view.begin(), data);
EXPECT_EQ(view.end(), data + sizeof(data));
}

TEST(MemoryViewTest, ArraySizeDeduction)
{
    char data[] = { 0x01, 0x02, 0x03 };
    MemoryView view { data };
    EXPECT_EQ(view.size(), 3);
    EXPECT_EQ(view.begin(), data);
    EXPECT_EQ(view.end(), data + sizeof(data));
}

TEST(MemoryViewTest, OperatorStringView)
{
ASSERT_FALSE((std::is_convertible_v<MemoryView<uint8_t>, std::string_view>));
ASSERT_TRUE((std::is_convertible_v<MemoryView<char>, std::string_view>));
ASSERT_TRUE((std::is_convertible_v<MemoryView<const char>, std::string_view>));
char data[] = {  'A', '0', '1' };
MemoryView view { data };
std::string_view result = view;
ASSERT_EQ(result.size(), 3);
EXPECT_EQ(result, "A01");
}

TEST(MemoryViewTest, OperatorIndex)
{
uint8_t data[] = { 0x01, 0x02, 0x03 };
MemoryView view { data, sizeof(data) };
ASSERT_TRUE((std::is_same_v<decltype(view)::value_type, uint8_t>));
EXPECT_EQ(view[0], 0x01);
EXPECT_EQ(view[1], 0x02);
EXPECT_EQ(view[2], 0x03);
}

TEST(MemoryViewTest, OperatorIndexConst)
{
const uint8_t data[] = { 0x01, 0x02, 0x03 };
MemoryView view { data, sizeof(data) };
ASSERT_TRUE((std::is_same_v<decltype(view)::value_type, const uint8_t>));
EXPECT_EQ(view[0], 0x01);
EXPECT_EQ(view[1], 0x02);
EXPECT_EQ(view[2], 0x03);
}
