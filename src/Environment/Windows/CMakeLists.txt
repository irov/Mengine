MENGINE_PROJECT(Windows)

ADD_FILTER(
src
    WindowsIncluder.h
    
    Win32PlatformServiceExtensionInterface.h
    
    DbgHelp.h
    
    Win32Helper.h
    Win32Helper.cpp
    
    Win32GetCallstack.h
    Win32GetCallstack.cpp
    
    Win32CreateProcess.h
    Win32CreateProcess.cpp
    
    Win32CreateProcessDump.h
    Win32CreateProcessDump.cpp
    
    Win32OutputDebugLogger.h
    Win32OutputDebugLogger.cpp
    
    Win32Register.h
    Win32Register.cpp
    
    Win32EnvironmentService.h
    Win32EnvironmentService.cpp
)

if(NOT MENGINE_TARGET_UWP)
    ADD_FILTER(
    src
        Win32FileHelper.h
        Win32FileHelper.cpp
    
        Win32MessageBoxLogger.h
        Win32MessageBoxLogger.cpp
    )
endif()

ADD_MENGINE_LIBRARY(Environment)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)