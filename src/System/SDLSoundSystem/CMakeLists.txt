set( MY_LIB_NAME SDLSoundSystem )

ADD_FILTER(
src
    SDLSoundBuffer.cpp
    SDLSoundBuffer.h
    SDLSoundBufferMemory.cpp
    SDLSoundBufferMemory.h
    SDLSoundBufferStream.cpp
    SDLSoundBufferStream.h
    SDLSoundConfig.h
    SDLSoundError.cpp
    SDLSoundError.h
    SDLSoundFilter.h
    SDLSoundSource.cpp
    SDLSoundSource.h
    SDLSoundSystem.cpp
    SDLSoundSystem.h
)

INCLUDE_DIRECTORIES(
    ${THIRDPARTY_DIR}/SDL2/include
    ${THIRDPARTY_DIR}/SDL2_mixer
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
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
    PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
    ${APPLICATION_LIB_PATHS}
    PARENT_SCOPE
)
