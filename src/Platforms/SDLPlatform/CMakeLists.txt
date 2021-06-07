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
    OSX/OSXOpenUrlInDefaultBrowser.h
    OSX/OSXOpenUrlInDefaultBrowser.m
    OSX/OSXSetDesktopWallpaper.h
    OSX/OSXSetDesktopWallpaper.m
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