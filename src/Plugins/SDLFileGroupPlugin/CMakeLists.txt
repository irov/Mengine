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

ADD_MENGINE_LIBRARY(Plugins)

if(MENGINE_TOOLCHAIN_XCODE_MAC)
    IF(MENGINE_BUILD_TYPE_DEBUG)
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/SDL2/Xcode/SDL/build/Debug/libSDL2.a)
    ELSE()
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/SDL2/Xcode/SDL/build/Release/libSDL2.a)
    ENDIF()
endif()