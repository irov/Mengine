MENGINE_PROJECT(MacOSWebViewPlatform)

ADD_FILTER(
src
    MacOSWebViewPlatformService.h
    MacOSWebViewPlatformService.mm
)

ADD_MENGINE_LIBRARY(Platforms)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel WebViewBridgeService AppleWebViewPlatform Apple "-framework AppKit")
