cmake_minimum_required(VERSION 3.15)

if ("${COMPONENT_NAME}" STREQUAL "")
    project(general-support-library)
    # Add the library
    add_library(general-support-library STATIC
            BufferedOut.cpp
            PersistentStorage.cpp
            )
    add_library(general-support-library-graphics STATIC
            graphics/EmbeddedFont.cpp
            graphics/EmbeddedFonts.cpp
            graphics/Canvas.cpp
    )
    target_compile_features(general-support-library PUBLIC cxx_std_17)
    target_compile_options(general-support-library PRIVATE -Wall -Wextra -pedantic)
    target_include_directories(general-support-library PUBLIC .)
    target_link_libraries(general-support-library-graphics PUBLIC general-support-library)
    set_source_files_properties(graphics/EmbeddedFonts.cpp PROPERTIES COMPILE_FLAGS -Wno-missing-braces)

    find_package(Freetype)
    find_package(Iconv)
    if (FREETYPE_FOUND AND ICONV_FOUND)
        add_subdirectory(gfx-font-converter)
    endif ()

    find_package(GTest)
    if (GTEST_FOUND)
        enable_testing()
        add_subdirectory(tests)
    endif ()
else ()
    set(srcs
            BufferedOut.cpp
            PersistentStorage.cpp
            graphics/EmbeddedFont.cpp
            graphics/EmbeddedFonts.cpp
            graphics/Canvas.cpp
            esp32-esp-idf/AnalogPin.cpp
            esp32-esp-idf/Delays.cpp
            esp32-esp-idf/GpioDigitalPin.cpp
            esp32-esp-idf/I2CBus.cpp
            esp32-esp-idf/I2CDevice.cpp
            esp32-esp-idf/PacketUart.cpp
            esp32-esp-idf/SerialDebug.cpp
            esp32-esp-idf/SerialOut.cpp
            esp32-esp-idf/SimpleUartDevice.cpp
            esp32-esp-idf/SpiBus.cpp
            esp32-esp-idf/SpiDevice.cpp
            )
    set(includedirs
            ${CMAKE_CURRENT_LIST_DIR}
            )
    set(private_requirements)
    if (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND private_requirements esp_adc driver esp_timer)
    endif ()
    idf_component_register(INCLUDE_DIRS ${includedirs}
            SRCS ${srcs}
            PRIV_REQUIRES ${private_requirements}
            )
    set_source_files_properties(graphics/EmbeddedFonts.cpp PROPERTIES COMPILE_FLAGS -Wno-missing-braces)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options(${COMPONENT_LIB} PUBLIC -DDEBUG_SERIAL_OUT)
    endif ()

endif ()

