MENGINE_PROJECT(OptickPlugin)

ADD_FILTER(
src
    OptickPlugin.cpp
    OptickPlugin.h
    OptickPlugin.def

    OptickProfilerSystem.cpp
    OptickProfilerSystem.h
    
    OptickThreadProfiler.cpp
    OptickThreadProfiler.h
    OptickFrameProfiler.cpp
    OptickFrameProfiler.h
    OptickCategoryProfiler.cpp
    OptickCategoryProfiler.h
    
    OptickProfilerDescription.cpp
    OptickProfilerDescription.h
)

include_directories(${THIRDPARTY_DIR}/optick/src)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_OPTICK)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}OptickCore${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)