MENGINE_PROJECT(SDLFileGroupPlugin)

ADD_FILTER(
src
    SDLFileGroupPlugin.h
    SDLFileGroupPlugin.cpp
    SDLFileGroupDirectory.cpp
    SDLFileGroupDirectory.h
	SDLFileGroupDirectoryFactory.h
	SDLFileGroupDirectoryFactory.cpp
    SDLFileInputStream.h
    SDLFileInputStream.cpp
    SDLFileOutputStream.cpp
    SDLFileOutputStream.h
    SDLFileMappedStream.h
    SDLFileMappedStream.cpp
	SDLFileHelper.h
	SDLFileHelper.cpp
)

if(MENGINE_TARGET_ANDROID)
    ADD_FILTER(
    android
        AndroidAssetServiceInterface.h
        AndroidAssetService.h
        AndroidAssetService.cpp
    )
endif()    

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SDLFILEGROUP)