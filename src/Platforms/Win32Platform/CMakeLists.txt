MENGINE_PROJECT(Win32Platform)

ADD_FILTER(
src
    Win32Platform.h
    Win32Platform.cpp
    Win32DynamicLibrary.h
    Win32DynamicLibrary.cpp
    Win32MouseEvent.h
    Win32MouseEvent.cpp
    Win32DateTimeProvider.h
    Win32DateTimeProvider.cpp
    Win32AlreadyRunningMonitor.h
    Win32AlreadyRunningMonitor.cpp
    Win32CPUInfo.h
    Win32CPUInfo.cpp
)

if(MENGINE_PLUGIN_DEVTODEBUG)
    ADD_FILTER(
    DevToDebug
        Win32PlatformDevToDebug.h
        Win32PlatformDevToDebug.cpp
    )
endif()

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

IF(MENGINE_PLUGIN_WIN32_FILEGROUP_STATIC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Win32FileGroupPlugin)
ENDIF()

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Wtsapi32.lib)
ENDIF()