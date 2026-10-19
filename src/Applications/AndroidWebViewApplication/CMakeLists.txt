MENGINE_PROJECT(AndroidWebViewApplication)

ADD_FILTER(
src
    AndroidWebViewApplication.h
    AndroidWebViewApplication.cpp
    AndroidWebViewMain.cpp
)

OPTION(MENGINE_WEBVIEW_MCP "Include the native WebView MCP test adapter" OFF)
IF(MENGINE_WEBVIEW_MCP)
    ADD_DEFINITIONS(-DMENGINE_WEBVIEW_MCP)
ENDIF()

ADD_MENGINE_EXECUTABLE_ANDROID()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} MengineWebViewRuntime AndroidWebViewPlatform Android Kernel)
