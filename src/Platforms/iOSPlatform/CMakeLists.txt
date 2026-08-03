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

if(MENGINE_RENDER_OPENGL)
    ADD_FILTER(
    src
        iOSOpenGLView.h
        iOSOpenGLView.mm
    )
elseif(MENGINE_RENDER_METAL)
    ADD_FILTER(
    src
        iOSMetalView.h
        iOSMetalView.mm
    )
endif()

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)
