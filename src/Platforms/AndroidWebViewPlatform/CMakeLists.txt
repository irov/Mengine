MENGINE_PROJECT(AndroidWebViewPlatform)

ADD_FILTER(
src
    AndroidWebViewPlatformService.h
    AndroidWebViewPlatformService.cpp
)

ADD_MENGINE_LIBRARY(Platforms)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android Kernel WebViewBridgeService)
