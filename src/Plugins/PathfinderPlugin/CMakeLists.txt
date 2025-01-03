MENGINE_PROJECT(PathfinderPlugin)

ADD_FILTER(
src
    PathfinderPlugin.h
    PathfinderPlugin.cpp
    PathfinderPlugin.def
    
    PathfinderInterface.h

    PathfinderService.cpp
    PathfinderService.h
    PathfinderMap.cpp
    PathfinderMap.h
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        PathfinderScriptEmbedding.cpp
        PathfinderScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_PATHFINDER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)