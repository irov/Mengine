MENGINE_PROJECT(SDLPlatform)

ADD_FILTER(
src
    SDLPlatform.h
    SDLPlatform.cpp
    SDLDynamicLibrary.h
    SDLDynamicLibrary.cpp
    SDLDateTimeProvider.h
    SDLDateTimeProvider.cpp
    SDLInput.h
    SDLInput.cpp
)

IF(MENGINE_TARGET_MACOS)
ADD_FILTER(
OSX
    OSX/OSXSetDesktopWallpaper.h
    OSX/OSXSetDesktopWallpaper.m
)
ENDIF()

IF(MENGINE_TARGET_APPLE)
ADD_FILTER(
Apple
    Apple/AppleOpenUrlInDefaultBrowser.h
    Apple/AppleOpenUrlInDefaultBrowser.m
)
ENDIF()

IF(MENGINE_TARGET_ANDROID)
ADD_FILTER(
Android
    Android/AndroidAssetFile.h
    Android/AndroidAssetFile.cpp
    Android/AndroidOpenUrlInDefaultBrowser.h
    Android/AndroidOpenUrlInDefaultBrowser.cpp
    Android/AndroidOpenMail.h
    Android/AndroidOpenMail.cpp
)
ENDIF()

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

IF(MENGINE_PLUGIN_SDL_FILEGROUP_STATIC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDLFileGroupPlugin)
ENDIF()

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)
ENDIF()