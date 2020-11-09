MENGINE_PROJECT(TTFPlugin)

ADD_FILTER(
src
    TTFPlugin.h
    TTFPlugin.cpp
    TTFPlugin.def
    TTFInterface.h
    TTFFont.h
    TTFFont.cpp
    TTFFontConfigLoader.h
    TTFFontConfigLoader.cpp
    TTFDataInterface.h
    TTFData.h
    TTFData.cpp
    TTFDataflow.h
    TTFDataflow.cpp
    TTFAtlasService.h
    TTFAtlasService.cpp
    TTFPrototypeGenerator.h
    TTFPrototypeGenerator.cpp
    TTFFontTextureGlyphProvider.h
    TTFFontTextureGlyphProvider.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/freetype/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_TTF)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}freetype${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)