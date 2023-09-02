#include <numeric>
#include "PersistentStorage.h"
#include "gtest/gtest.h"

namespace
{
    constexpr int BufferSize = 1024;

    struct TestObj
    {
        int x;
        int y;
        int z;
        bool operator==(const TestObj & other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }
    };
}

using embedded::PersistentStorage;
using embedded::BytesView;

//Test set() and get() with a new index:
TEST(PersistentStorageTest, SetAndGetNewObject)
{
    
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { buffer };

    TestObj testObject = { 1, 2, 3 };
    std::string_view testIndex = "test";
    auto result = storage.set(testIndex, testObject);

    // Ensure that set() returns true
    EXPECT_TRUE(result);

    // Ensure that get() returns the same object
    auto retrievedObject = storage.get<TestObj>(testIndex);
    ASSERT_TRUE(retrievedObject);
    EXPECT_EQ(*retrievedObject, testObject);
}

//Test set() and get() with an existing index:
TEST(PersistentStorageTest, SetAndGetExistingObject)
{
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    TestObj testObject1 = { 1, 2, 3 };
    std::string_view testIndex = "test";
    auto result = storage.set(testIndex, testObject1);

    // Ensure that set() returns true
    EXPECT_TRUE(result);

    // Ensure that get() returns the same object
    auto retrievedObject1 = storage.get<TestObj>(testIndex);
    ASSERT_TRUE(retrievedObject1);
    EXPECT_EQ(*retrievedObject1, testObject1);

    TestObj testObject2 = { 4, 5, 6 };
    result = storage.set(testIndex, testObject2);

    // Ensure that set() returns true
    EXPECT_TRUE(result);

    // Ensure that get() returns the new object
    auto retrievedObject2 = storage.get<TestObj>(testIndex);
    ASSERT_TRUE(retrievedObject2);
    EXPECT_EQ(*retrievedObject2, testObject2);
}

//Test set() with not enough space:
TEST(PersistentStorageTest, SetNotEnoughSpace)
{
    constexpr int smallBufferSize = 6 + sizeof(TestObj) - 1; // 6 is a size of NodeDescriptor
    std::array<uint8_t, smallBufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    TestObj testObject = { 1, 2, 3 };
    std::string_view testIndex = "key";
    auto result = storage.set(testIndex, testObject);

    EXPECT_FALSE(result);
}

//Test get() with non-existing index:
TEST(PersistentStorageTest, GetNonExistingIndex)
{
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    std::string_view testIndex = "key";
    auto retrievedObject = storage.get<int>(testIndex);

    EXPECT_FALSE(retrievedObject);
}

//Test get() with wrong type:
TEST(PersistentStorageTest, GetWrongType)
{
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    TestObj testObject = { 1, 2, 3 };
    std::string_view testIndex = "key";
    storage.set(testIndex, testObject);

    // Try to retrieve the object as another type
    auto retrievedObject = storage.get<double>(testIndex);

    // Ensure that get() returns nullopt
    EXPECT_FALSE(retrievedObject);
}

//Test get() with wrong size:
TEST(PersistentStorageTest, GetSetTest)
{
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    const std::string_view key1 = "key1";
    const int value1 = 123;
    storage.set(key1, value1);

    const std::string_view key2 = "key2";
    const float value2 = 3.14159f;
    storage.set(key2, value2);

    const std::string key3 = "key3";
    const std::string value3 = "Hello World!";
    storage.set(key3, value3);

    ASSERT_EQ(value1, storage.get<int>(key1).value_or(0));
    ASSERT_EQ(value2, storage.get<float>(key2).value_or(0.0f));
    ASSERT_EQ(value3, storage.get<std::string>(key3).value_or(""));
}

//Test get() with wrong size:
TEST(PersistentStorageTest, SetWithExistingKey)
{
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    const std::string_view key = "key";
    const int value1 = 123;
    const int value2 = 456;

    ASSERT_TRUE(storage.set(key, value1));
    ASSERT_EQ(value1, storage.get<int>(key).value_or(0));
    ASSERT_TRUE(storage.set(key, value2));
    ASSERT_EQ(value2, storage.get<int>(key).value_or(0));
}

//Test get() with wrong size:
TEST(PersistentStorageTest, GetNotExistingKey)
{
    std::array<uint8_t, BufferSize> buffer {};
    PersistentStorage storage { BytesView(buffer) };

    const std::string_view key = "key";

    ASSERT_FALSE(storage.get<int>(key).has_value());
}

//Test get() with wrong size:
TEST(PersistentStorageTest, Persistence)
{
    std::array<uint8_t, BufferSize> buffer {};
    std::memset(buffer.data(), 127, buffer.size());
    PersistentStorage storage { BytesView(buffer) };

    ASSERT_EQ(std::accumulate(buffer.begin(), buffer.end(), 0u), 0);
    TestObj testObject1 = { 1, 2, 3 };
    std::string_view testIndex("key1");
    auto result = storage.set(testIndex, testObject1);

    // Ensure that set() returns true
    EXPECT_TRUE(result);

    // Ensure that get() returns the same object
    auto retrievedObject1 = storage.get<TestObj>(testIndex);
    ASSERT_TRUE(retrievedObject1);
    EXPECT_EQ(*retrievedObject1, testObject1);

    PersistentStorage storage2 { BytesView(buffer) , false };
    TestObj testObject2 = { 4, 5, 6 };
    std::string_view testIndex2("key2");
    result = storage2.set(testIndex2, testObject2);

    // Ensure that set() returns true
    EXPECT_TRUE(result);

    // Ensure that get() returns the new object
    auto retrievedObject2 = storage.get<TestObj>(testIndex);
    ASSERT_TRUE(retrievedObject2);
    EXPECT_EQ(*retrievedObject2, testObject1);

    retrievedObject2 = storage2.get<TestObj>(testIndex2);
    ASSERT_TRUE(retrievedObject2);
    EXPECT_EQ(*retrievedObject2, testObject2);
}
