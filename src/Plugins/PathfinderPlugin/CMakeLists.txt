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

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        PathfinderScriptEmbedding.cpp
        PathfinderScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_PATHFINDER)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)