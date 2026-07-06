MENGINE_PROJECT(VideoPlugin)

ADD_FILTER(
src
    VideoInterface.h

    VideoPlugin.h
    VideoPlugin.cpp
    VideoPlugin.def

    ResourceVideo.cpp
    ResourceVideo.h

    SurfaceVideo.cpp
    SurfaceVideo.h
    SurfaceMockupVideo.cpp
    SurfaceMockupVideo.h

    ResourceVideoValidator.cpp
    ResourceVideoValidator.h
)

if(MENGINE_PLUGIN_METABUF)
    ADD_FILTER(
    Metabuf
        MetabufLoaderResourceVideo.cpp
        MetabufLoaderResourceVideo.h
    )

    add_definitions(-DMETABUF_EXTERNAL_CONFIG)

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
endif()

if(MENGINE_PLUGIN_JSON)
    ADD_FILTER(
    JSONLoader
        JSONLoaderResourceVideo.cpp
        JSONLoaderResourceVideo.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        VideoScriptEmbedding.cpp
        VideoScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_VIDEO)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLUGIN_METABUF)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
endif()
