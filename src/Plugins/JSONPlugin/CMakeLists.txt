MENGINE_PROJECT(JSONPlugin)

ADD_FILTER(
src
    JSONInterface.h

    JSONPlugin.h
    JSONPlugin.cpp
    JSONPlugin.def

    JSONService.h
    JSONService.cpp

    JSONPackageLoader.h
    JSONPackageLoader.cpp
    JSONPackageContentLoader.h
    JSONPackageContentLoader.cpp

    JSONLoaderResourceHelper.cpp
    JSONLoaderResourceHelper.h
    JSONLoaderResourceCursorICO.cpp
    JSONLoaderResourceCursorICO.h
    JSONLoaderResourceCursorSystem.cpp
    JSONLoaderResourceCursorSystem.h
    JSONLoaderResourceFile.cpp
    JSONLoaderResourceFile.h
    JSONLoaderResourceHIT.cpp
    JSONLoaderResourceHIT.h
    JSONLoaderResourceImageData.cpp
    JSONLoaderResourceImageData.h
    JSONLoaderResourceImageDefault.cpp
    JSONLoaderResourceImageDefault.h
    JSONLoaderResourceImageEmpty.cpp
    JSONLoaderResourceImageEmpty.h
    JSONLoaderResourceImageSequence.cpp
    JSONLoaderResourceImageSequence.h
    JSONLoaderResourceImageSolid.cpp
    JSONLoaderResourceImageSolid.h
    JSONLoaderResourceImageSubstract.cpp
    JSONLoaderResourceImageSubstract.h
    JSONLoaderResourceImageSubstractRGBAndAlpha.cpp
    JSONLoaderResourceImageSubstractRGBAndAlpha.h
    JSONLoaderResourceJSON.cpp
    JSONLoaderResourceJSON.h
    JSONLoaderResourceMusic.cpp
    JSONLoaderResourceMusic.h
    JSONLoaderResourceShape.cpp
    JSONLoaderResourceShape.h
    JSONLoaderResourceSound.cpp
    JSONLoaderResourceSound.h
    JSONLoaderResourceWindow.cpp
    JSONLoaderResourceWindow.h

    ResourceJSON.cpp
    ResourceJSON.h

    JSONSetting.cpp
    JSONSetting.h
    JSONSettingPrototypeGenerator.cpp
    JSONSettingPrototypeGenerator.h

    JSONConfig.cpp
    JSONConfig.h
)

if(MENGINE_PLUGIN_METABUF)
    ADD_FILTER(
    Metabuf
        MetabufLoaderResourceJSON.cpp
        MetabufLoaderResourceJSON.h
    )

    add_definitions(-DMETABUF_EXTERNAL_CONFIG)

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        JSONScriptEmbedding.cpp
        JSONScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jansson/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_JSON)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLUGIN_METABUF)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jpp${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}json${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jansson${MENGINE_LIB_SUFFIX})
