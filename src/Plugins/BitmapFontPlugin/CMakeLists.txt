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
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/xmlsax/include)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}xmlsax${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
