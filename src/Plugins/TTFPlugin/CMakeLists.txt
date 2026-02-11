MENGINE_PROJECT(TTFPlugin)

ADD_FILTER(
src
    TTFIncluder.h
    TTFPlugin.h
    TTFPlugin.cpp
    TTFPlugin.def
    TTFInterface.h
    TTFFont.h
    TTFFont.cpp
    TTFFontGlyph.h
    TTFFontGlyph.cpp
    TTFFontConfigLoader.h
    TTFFontConfigLoader.cpp
    TTFFontGlyphConfigLoader.h
    TTFFontGlyphConfigLoader.cpp
    TTFFontValidator.h
    TTFFontValidator.cpp
    TTFFontGlyphValidator.h
    TTFFontGlyphValidator.cpp
    TTFDataInterface.h
    TTFData.h
    TTFData.cpp
    TTFDataflow.h
    TTFDataflow.cpp
    TTFAtlasService.h
    TTFAtlasService.cpp
    TTFFontPrototypeGenerator.h
    TTFFontPrototypeGenerator.cpp
    TTFFontGlyphPrototypeGenerator.h
    TTFFontGlyphPrototypeGenerator.cpp
    TTFFontTextureGlyphProvider.h
    TTFFontTextureGlyphProvider.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/freetype/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_TTF)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}freetype${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)