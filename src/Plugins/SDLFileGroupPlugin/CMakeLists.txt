set(MY_LIB_NAME SDLFileGroupPlugin)

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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Plugins)