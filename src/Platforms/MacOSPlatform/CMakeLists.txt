MENGINE_PROJECT(MacOSPlatform)

if(NOT MENGINE_RENDER_METAL)
    message(FATAL_ERROR "MacOSPlatform supports Metal only")
endif()

ADD_FILTER(
src
    MacOSPlatformService.h
    MacOSPlatformService.mm
    MacOSPlatformSystem.h
    MacOSPlatformSystem.mm
    MacOSInput.h
    MacOSInput.mm
    MacOSMetalView.h
    MacOSMetalView.mm
    MacOSWindowDelegate.h
    MacOSWindowDelegate.mm
)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} MacOS)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)

ADD_MENGINE_APPLE_FRAMEWORK(AppKit)
ADD_MENGINE_APPLE_FRAMEWORK(Foundation)
ADD_MENGINE_APPLE_FRAMEWORK(Carbon)
ADD_MENGINE_APPLE_FRAMEWORK(CoreGraphics)
ADD_MENGINE_APPLE_FRAMEWORK(Metal)
ADD_MENGINE_APPLE_FRAMEWORK(QuartzCore)
