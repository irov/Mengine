MENGINE_PROJECT(MetabufPlugin)

ADD_FILTER(
src
    MetabufPackageLoader.cpp
    MetabufPackageLoader.h
    
    MetabufLoaderResourceCursorICO.cpp
    MetabufLoaderResourceCursorICO.h
    MetabufLoaderResourceCursorSystem.cpp
    MetabufLoaderResourceCursorSystem.h
    MetabufLoaderResourceFile.cpp
    MetabufLoaderResourceFile.h
    MetabufLoaderResourceHIT.cpp
    MetabufLoaderResourceHIT.h
    MetabufLoaderResourceImageData.cpp
    MetabufLoaderResourceImageData.h
    MetabufLoaderResourceImageDefault.cpp
    MetabufLoaderResourceImageDefault.h
    MetabufLoaderResourceImageEmpty.cpp
    MetabufLoaderResourceImageEmpty.h
    MetabufLoaderResourceImageSequence.cpp
    MetabufLoaderResourceImageSequence.h
    MetabufLoaderResourceImageSolid.cpp
    MetabufLoaderResourceImageSolid.h
    MetabufLoaderResourceImageSubstract.cpp
    MetabufLoaderResourceImageSubstract.h
    MetabufLoaderResourceImageSubstractRGBAndAlpha.cpp
    MetabufLoaderResourceImageSubstractRGBAndAlpha.h
    MetabufLoaderResourceMusic.cpp
    MetabufLoaderResourceMusic.h
    MetabufLoaderResourceShape.cpp
    MetabufLoaderResourceShape.h
    MetabufLoaderResourceSound.cpp
    MetabufLoaderResourceSound.h
    MetabufLoaderResourceWindow.cpp
    MetabufLoaderResourceWindow.h

    MetabufPlugin.cpp
    MetabufPlugin.h
    MetabufPlugin.def
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_METABUF)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)