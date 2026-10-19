MENGINE_PROJECT(Win32WebViewPlatform)

INCLUDE(FetchContent)
FetchContent_Declare(MengineWebView2SDK
    URL https://api.nuget.org/v3-flatcontainer/microsoft.web.webview2/1.0.3537.50/microsoft.web.webview2.1.0.3537.50.nupkg
    DOWNLOAD_NAME Microsoft.Web.WebView2.zip
    URL_HASH SHA256=5ea526bbd728adda0da4d31219267e96460494a427e4894c4e09d9f320f4b9aa
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_MakeAvailable(MengineWebView2SDK)

ADD_FILTER(
src
    Win32WebViewIncluder.h
    Win32WebViewPlatformService.h
    Win32WebViewPlatformService.cpp
)

ADD_MENGINE_LIBRARY(Platforms)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} SYSTEM PUBLIC ${menginewebview2sdk_SOURCE_DIR}/build/native/include)
IF(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
    SET(WebView2Architecture arm64)
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(WebView2Architecture x64)
ELSE()
    SET(WebView2Architecture x86)
ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel Windows WebViewBridgeService
    ${menginewebview2sdk_SOURCE_DIR}/build/native/${WebView2Architecture}/WebView2LoaderStatic.lib
    ole32 oleaut32 shlwapi version wininet)
