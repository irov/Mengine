MENGINE_PROJECT(AndroidPlatform)

ADD_FILTER(
src
    AndroidPlatformService.h
    AndroidPlatformService.cpp
    
    AndroidPlatformSystem.h
    AndroidPlatformSystem.cpp
    
    AndroidAnalyticsEventProvider.h
    AndroidAnalyticsEventProvider.cpp
    
    AndroidProxyLogger.h
    AndroidProxyLogger.cpp
    
    AndroidDynamicLibrary.h
    AndroidDynamicLibrary.cpp
)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)

find_library(EGL-lib EGL)

target_link_libraries(${PROJECT_NAME} ${EGL-lib})