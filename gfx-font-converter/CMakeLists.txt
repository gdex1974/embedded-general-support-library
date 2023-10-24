add_executable(gfx-font-converter
        main.cpp
        FontConverterConfig.cpp
        EncodingConverter.cpp
        FreeTypeLibrary.cpp
        FreeTypeLibrary.h
        FreeTypeFace.cpp
        FreeTypeFace.h
        FreeTypeGlyph.cpp
        FreeTypeGlyph.h
)
target_link_libraries(gfx-font-converter
        PRIVATE
        general-support-library-graphics
        Freetype::Freetype
        Iconv::Iconv
)
target_compile_features(gfx-font-converter PUBLIC cxx_std_17)
target_compile_options(gfx-font-converter PRIVATE -Wall -Wextra -pedantic)
