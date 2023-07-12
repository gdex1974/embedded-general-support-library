cmake_minimum_required(VERSION 3.10)

include(GoogleTest)
# Add tests executable
add_executable(tests
        BaseGeometryTest.cpp
        BufferedOutTest.cpp
        EndianConversionTest.cpp
        MemoryViewTest.cpp
        PersistentStorageTest.cpp
)

target_link_libraries(tests
        PRIVATE general-support-library
        GTest::GTest
        GTest::Main
)
gtest_discover_tests(tests)