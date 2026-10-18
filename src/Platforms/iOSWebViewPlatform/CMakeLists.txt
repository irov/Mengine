MENGINE_PROJECT(iOSWebViewPlatform)

ADD_FILTER(
src
    iOSWebViewPlatformService.h
    iOSWebViewPlatformService.mm
)

ADD_MENGINE_LIBRARY(Platforms)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel WebViewBridgeService AppleWebViewPlatform "-framework UIKit" "-framework WebKit")
