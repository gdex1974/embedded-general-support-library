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

TEST(MemoryViewTest, ConstCopyConstructor)
{
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    embedded::BytesView view { data, sizeof(data) };
    embedded::ConstBytesView constView = view;
    EXPECT_EQ(constView.size(), view.size());
    EXPECT_EQ(constView.begin(), view.begin());
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
    char data[] = { 'A', '0', '1' };
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
    ASSERT_TRUE((std::is_same_v<decltype(view)::element_type, const uint8_t>));
    ASSERT_TRUE((std::is_same_v<decltype(view)::value_type, uint8_t>));
    EXPECT_EQ(view[0], 0x01);
    EXPECT_EQ(view[1], 0x02);
    EXPECT_EQ(view[2], 0x03);
}

TEST(MemoryViewTest, BeginEnd) {
    int arr[] = {1, 2, 3, 4, 5};
    MemoryView<int> view(arr);
    auto it = view.begin();
    auto cit = view.cbegin();
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(*cit, *it);
    ++it;
    EXPECT_EQ(*it, 2);
    ++cit;
    EXPECT_EQ(*cit, *it);

    auto endIt = view.end();
    --endIt;
    EXPECT_EQ(*endIt, 5);

    auto cendIt = view.cend();
    --cendIt;
    EXPECT_EQ(*cendIt, *endIt);
}

TEST(MemoryViewTest, RbeginRend) {
    int arr[] = {1, 2, 3, 4, 5};
    MemoryView<int> view(arr);
    auto it = view.rbegin();
    auto cit = view.crbegin();
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(*cit, *it);
    ++it;
    EXPECT_EQ(*it, 4);
    ++cit;
    EXPECT_EQ(*cit, *it);

    auto endIt = view.rend();
    --endIt;
    EXPECT_EQ(*endIt, 1);

    auto cendIt = view.crend();
    --cendIt;
    EXPECT_EQ(*cendIt, *endIt);
}

TEST(MemoryViewTest, Subspan) {
    int arr[] = {1, 2, 3, 4, 5};
    MemoryView<int> view(arr);
    auto subView = view.subspan(1, 3);
    EXPECT_EQ(subView.size(), 3);
    EXPECT_EQ(subView[0], 2);
    EXPECT_EQ(subView[2], 4);
    subView = view.subspan(3);
    EXPECT_FALSE(subView.empty());
    EXPECT_EQ(subView.size(), 2);
    EXPECT_EQ(subView[0], 4);
    EXPECT_EQ(subView[1], 5);
    EXPECT_TRUE(view.subspan(5).empty());
    EXPECT_TRUE(view.subspan(6).empty());
}

TEST(MemoryViewTest, AsBytes) {
    const int arr[] = {1, 2, 3, 4, 5};
    MemoryView view(arr);
    const auto bytes = view.as_bytes();
    EXPECT_EQ(bytes.size(), sizeof(int) * 5);
    EXPECT_EQ(bytes[0], 1);
    EXPECT_EQ(bytes[sizeof(int)], 2);
}

TEST(MemoryViewTest, AsWritableBytes) {
    int arr[] = {1, 2, 3, 4, 5};
    MemoryView view(arr);
    auto writableBytes = view.as_writable_bytes();
    EXPECT_EQ(writableBytes.size(), sizeof(int) * 5);
    writableBytes[sizeof(int)] = 10;
    EXPECT_EQ(view[1], 10);
}

TEST(MemoryViewTest, First) {
    int arr[] = {1, 2, 3, 4, 5};
    MemoryView<int> view(arr);
    auto first = view.first(3);
    EXPECT_EQ(first.size(), 3);
    EXPECT_EQ(first[0], 1);
    first = view.first(5);
    EXPECT_EQ(first.size(), 5);
    EXPECT_EQ(first[0], 1);
    first = view.first(6);
    EXPECT_EQ(first.size(), 5);
    EXPECT_EQ(first[0], 1);
}

TEST(MemoryViewTest, Last) {
    int arr[] = {1, 2, 3, 4, 5};
    MemoryView<int> view(arr);
    auto last = view.last(3);
    EXPECT_EQ(last.size(), 3);
    EXPECT_EQ(last[0], 3);
    last = view.last(5);
    EXPECT_EQ(last.size(), 5);
    EXPECT_EQ(last[0], 1);
    last = view.last(6);
    EXPECT_EQ(last.size(), 5);
    EXPECT_EQ(last[0], 1);
}