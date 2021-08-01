MENGINE_PROJECT(MetabufLoaderPlugin)

ADD_FILTER(
src
    LoaderResourceCursorICO.cpp
    LoaderResourceCursorICO.h
    LoaderResourceCursorSystem.cpp
    LoaderResourceCursorSystem.h
    LoaderResourceFile.cpp
    LoaderResourceFile.h
    LoaderResourceHIT.cpp
    LoaderResourceHIT.h
    LoaderResourceImageData.cpp
    LoaderResourceImageData.h
    LoaderResourceImageDefault.cpp
    LoaderResourceImageDefault.h
    LoaderResourceImageEmpty.cpp
    LoaderResourceImageEmpty.h    
    LoaderResourceImageSequence.cpp
    LoaderResourceImageSequence.h
    LoaderResourceImageSolid.cpp
    LoaderResourceImageSolid.h
    LoaderResourceImageSubstract.cpp
    LoaderResourceImageSubstract.h
    LoaderResourceImageSubstractRGBAndAlpha.cpp
    LoaderResourceImageSubstractRGBAndAlpha.h
    LoaderResourceMusic.cpp
    LoaderResourceMusic.h
    LoaderResourceShape.cpp
    LoaderResourceShape.h
    LoaderResourceSound.cpp
    LoaderResourceSound.h
    LoaderResourceWindow.cpp
    LoaderResourceWindow.h
    MetabufLoaderPlugin.cpp
    MetabufLoaderPlugin.h    
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/metabuf/include )

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_METABUFLOADER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)