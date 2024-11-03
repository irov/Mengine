MENGINE_PROJECT(SDLFileSystem)

ADD_FILTER(
src
    SDLFileSystem.h
    SDLFileSystem.cpp
    
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

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL/include)

ADD_MENGINE_LIBRARY(Systems)