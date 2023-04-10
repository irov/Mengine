MENGINE_PROJECT(Windows)

ADD_FILTER(
src
    WindowsIncluder.h
    
    Win32Helper.h
    Win32Helper.cpp
    
    Win32OutputDebugLogger.h
    Win32OutputDebugLogger.cpp
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