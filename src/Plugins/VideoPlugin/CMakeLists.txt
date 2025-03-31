MENGINE_PROJECT(VideoPlugin)

ADD_FILTER(
src
    VideoInterface.h

    VideoPlugin.h
    VideoPlugin.cpp
    VideoPlugin.def
    
    ResourceVideo.cpp
    ResourceVideo.h
    
    LoaderResourceVideo.cpp
    LoaderResourceVideo.h

    SurfaceVideo.cpp
    SurfaceVideo.h
    SurfaceMockupVideo.cpp
    SurfaceMockupVideo.h

    ResourceVideoValidator.cpp
    ResourceVideoValidator.h
)
    
if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        VideoScriptEmbedding.cpp
        VideoScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_VIDEO)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)