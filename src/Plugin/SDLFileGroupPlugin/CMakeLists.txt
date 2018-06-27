set( MY_LIB_NAME SDLFileGroupPlugin )

ADD_FILTER(
src
    SDLFileGroupPlugin.h
    SDLFileGroupPlugin.cpp
    SDLFileGroupDirectory.cpp
    SDLFileGroupDirectory.h
    SDLFileInputStream.h
    SDLFileInputStream.cpp
    SDLFileOutputStream.cpp
    SDLFileOutputStream.h
    SDLFileMappedStream.h
    SDLFileMappedStream.cpp
)

INCLUDE_DIRECTORIES(
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/SDL2/include
    ${THIRDPARTY_DIR}/libmath/include
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

SET( APPLICATION_DEPENDENCIES
    ${APPLICATION_DEPENDENCIES}
    ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
    ${APPLICATION_DEPENDENCIES}
    PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Logger
    Core
    Codec
    Archive
)

SET( APPLICATION_LIB_PATHS
    ${APPLICATION_LIB_PATHS}
    PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    PARENT_SCOPE
)
