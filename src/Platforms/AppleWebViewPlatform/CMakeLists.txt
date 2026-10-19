MENGINE_PROJECT(AppleWebViewPlatform)

ADD_FILTER(
src
    AppleWebViewController.h
    AppleWebViewMCPApplicationInterface.h
    AppleWebViewController.mm
)

ADD_MENGINE_LIBRARY(Platforms)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel WebViewBridgeService "-framework WebKit")
IF(MENGINE_TARGET_MACOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-framework AppKit" "-framework QuartzCore")
ELSE()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-framework UIKit")
ENDIF()
