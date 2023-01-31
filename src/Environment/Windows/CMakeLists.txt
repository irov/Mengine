MENGINE_PROJECT(Windows)

ADD_FILTER(
src
    WindowsIncluder.h
    
    Win32Helper.h
    Win32Helper.cpp
    
    Win32FileHelper.h
    Win32FileHelper.cpp
)

ADD_MENGINE_LIBRARY(Environment)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)