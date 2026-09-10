MENGINE_PROJECT(iOSPlatform)

OPTION(MENGINE_PLATFORM_IOS_ACCELEROMETER "Enable iOS accelerometer input" OFF)

IF(MENGINE_PLATFORM_IOS_ACCELEROMETER)
    MENGINE_ADD_DEFINITION(MENGINE_PLATFORM_IOS_ACCELEROMETER)
ENDIF()

ADD_FILTER(
src
    iOSPlatformService.h
    iOSPlatformService.mm
    iOSPlatformSystem.h
    iOSPlatformSystem.mm
    iOSInput.h
    iOSInput.mm
    
    iOSMailComposeDelegate.h
    iOSMailComposeDelegate.mm
    
    iOSAnalyticsEventProvider.h
    iOSAnalyticsEventProvider.mm
    iOSAttributionProvider.h
    iOSAttributionProvider.mm
)

ADD_FILTER(
src
    iOSMetalView.h
    iOSMetalView.mm
)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)
