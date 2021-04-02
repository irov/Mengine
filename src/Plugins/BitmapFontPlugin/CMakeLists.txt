MENGINE_PROJECT(BitmapFontPlugin)

ADD_FILTER(
src
    BitmapFontPlugin.h
    BitmapFontPlugin.cpp
    BitmapFontInterface.h
    BitmapFont.h
    BitmapFont.cpp
    BitmapFontConfigLoader.h
    BitmapFontConfigLoader.cpp
    BitmapGlyph.h
    BitmapGlyph.cpp
    BitmapGlyphService.h
    BitmapGlyphService.cpp
    BitmapFontPrototypeGenerator.h
    BitmapFontPrototypeGenerator.cpp
    BitmapFontValidator.h
    BitmapFontValidator.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/xmlsax/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_BITMAPFONT)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}xmlsax${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
