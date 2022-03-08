MENGINE_PROJECT(SDLFileGroupPlugin)

ADD_FILTER(
src
    SDLFileGroupPlugin.h
    SDLFileGroupPlugin.cpp
    SDLFileGroupPlugin.def
    
    SDLFileGroupDirectory.cpp
    SDLFileGroupDirectory.h
	SDLFileGroupDirectoryFactory.h
	SDLFileGroupDirectoryFactory.cpp
    SDLFileInputStream.h
    SDLFileInputStream.cpp
    SDLMutexFileInputStream.h
    SDLMutexFileInputStream.cpp
    SDLFileOutputStream.cpp
    SDLFileOutputStream.h
    SDLFileMapped.h
    SDLFileMapped.cpp
	SDLFileHelper.h
	SDLFileHelper.cpp
)

include_directories(${THIRDPARTY_CONFIG_DIR}/SDL)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SDLFILEGROUP)