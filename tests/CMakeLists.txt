cmake_minimum_required(VERSION 3.10)

include(GoogleTest)
# Add tests executable
add_executable(tests
        BufferedOutTest.cpp
        EndianConversionTest.cpp
        MemoryViewTest.cpp
        PersistentStorageTest.cpp
)

add_executable(graphics-tests
        BaseGeometryTest.cpp
        MonochromeFrameBufferTest.cpp
        PBMHelper.cpp
)
target_link_libraries(tests
        PRIVATE general-support-library
        GTest::GTest
        GTest::Main
)

target_link_libraries(graphics-tests
        PRIVATE general-support-library-graphics
        GTest::GTest
        GTest::Main
)
add_test(NAME base-tests COMMAND tests)
add_test(NAME graphics-tests COMMAND graphics-tests WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/data")
set_tests_properties(base-tests PROPERTIES TIMEOUT 10)
set_tests_properties(graphics-tests PROPERTIES TIMEOUT 10)
