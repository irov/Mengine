MENGINE_PROJECT(iOSPlatform)

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
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL3/include)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL3)