set(MY_LIB_NAME BitmapFontPlugin)

ADD_FILTER(
src
    BitmapFontPlugin.h
    BitmapFontPlugin.cpp
	BitmapFontInterface.h
    BitmapFont.h
    BitmapFont.cpp
    BitmapGlyph.h
    BitmapGlyph.cpp
	BitmapGlyphService.h
	BitmapGlyphService.cpp
	BitmapFontPrototypeGenerator.h
	BitmapFontPrototypeGenerator.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)