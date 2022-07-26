MENGINE_PROJECT(MemoryUsageMonitorPlugin)

ADD_FILTER(
src
    MemoryUsageMonitorPlugin.h
    MemoryUsageMonitorPlugin.cpp
    MemoryUsageMonitorPlugin.def
    
    MemoryUsageMonitor.h
    MemoryUsageMonitor.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MEMORYUSAGEMONITOR)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
