MENGINE_PROJECT(Win32AntifreezeMonitorPlugin)

ADD_FILTER(
src
    Win32AntifreezeMonitorPlugin.h
    Win32AntifreezeMonitorPlugin.cpp
    Win32AntifreezeMonitorPlugin.def
    
    Win32AntifreezeMonitor.h
    Win32AntifreezeMonitor.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
