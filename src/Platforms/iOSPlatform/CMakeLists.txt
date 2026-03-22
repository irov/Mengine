MENGINE_PROJECT(iOSPlatform)

ADD_FILTER(
src
    iOSPlatformService.h
    iOSPlatformService.mm
    iOSPlatformSystem.h
    iOSPlatformSystem.mm
    iOSInput.h
    iOSInput.mm
    
    iOSOpenGLView.h
    iOSOpenGLView.mm
    
    iOSMailComposeDelegate.h
    iOSMailComposeDelegate.mm
    
    iOSAnalyticsEventProvider.h
    iOSAnalyticsEventProvider.mm
)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)