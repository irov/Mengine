MENGINE_PROJECT(UWP)

ADD_FILTER(
src
    UWPIncluder.h
    
    UWPPlatformServiceExtensionInterface.h
    
    UWPHelper.h
    UWPHelper.cpp
    
    UWPOutputDebugLogger.h
    UWPOutputDebugLogger.cpp
    
    UWPEnvironmentService.h
    UWPEnvironmentService.cpp
)

ADD_MENGINE_LIBRARY(Environment)