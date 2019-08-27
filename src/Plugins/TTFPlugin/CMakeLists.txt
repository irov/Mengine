set(MY_LIB_NAME TTFPlugin)

ADD_FILTER(
src
    TTFPlugin.h
	TTFPlugin.cpp
    TTFServiceInterface.h
    TTFFont.h
    TTFFont.cpp
    TTFDataInterface.h
    TTFData.h
    TTFData.cpp
    TTFDataflow.h
    TTFDataflow.cpp
    FEDataInterface.h
    FEData.h
    FEData.cpp
    FEDataflow.h
    FEDataflow.cpp
    TTFAtlasService.h
    TTFAtlasService.cpp    
    TTFPrototypeGenerator.h
    TTFPrototypeGenerator.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/freetype/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libfe/include)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}freetype${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}font-effects${MENGINE_LIB_SUFFIX})