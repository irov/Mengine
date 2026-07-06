MENGINE_PROJECT(SpinePlugin)

ADD_FILTER(
src
    SpineInterface.h

    Spine.h
    Spine.cpp

    SamplerSpineAnimation.h
    SamplerSpineAnimation.cpp

    ResourceSpineSkeleton.h
    ResourceSpineSkeleton.cpp
    ResourceSpineAtlas.h
    ResourceSpineAtlasDefault.h
    ResourceSpineAtlasDefault.cpp
    ResourceSpineAtlasTexturepacker.h
    ResourceSpineAtlasTexturepacker.cpp

    SpinePlugin.h
    SpinePlugin.cpp
    SpinePlugin.def

    SpineDebuggerBoundingBox.h
    SpineDebuggerBoundingBox.cpp
)

if(MENGINE_PLUGIN_METABUF)
    ADD_FILTER(
    Metabuf
        MetabufLoaderResourceSpineAtlasDefault.h
        MetabufLoaderResourceSpineAtlasDefault.cpp
        MetabufLoaderResourceSpineAtlasTexturepacker.h
        MetabufLoaderResourceSpineAtlasTexturepacker.cpp
        MetabufLoaderResourceSpineSkeleton.h
        MetabufLoaderResourceSpineSkeleton.cpp
    )

    add_definitions(-DMETABUF_EXTERNAL_CONFIG)

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
endif()

if(MENGINE_PLUGIN_JSON)
    ADD_FILTER(
    JSONLoader
        JSONLoaderResourceSpineAtlasDefault.h
        JSONLoaderResourceSpineAtlasDefault.cpp
        JSONLoaderResourceSpineAtlasTexturepacker.h
        JSONLoaderResourceSpineAtlasTexturepacker.cpp
        JSONLoaderResourceSpineSkeleton.h
        JSONLoaderResourceSpineSkeleton.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        SpineScriptEmbedding.h
        SpineScriptEmbedding.cpp
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/spine/spine-c/spine-c/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SPINE)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLUGIN_METABUF)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}spine-c${MENGINE_LIB_SUFFIX})
