set( MY_LIB_NAME OpenALSoundSystem )

ADD_FILTER(
src
	OpenALSoundBufferBase.cpp
	OpenALSoundBufferBase.h
	OpenALSoundBufferMemory.cpp
	OpenALSoundBufferMemory.h
	OpenALSoundBufferStream.cpp
	OpenALSoundBufferStream.h
	OpenALSoundSource.cpp
	OpenALSoundSource.h
	OpenALSoundSystem.cpp
	OpenALSoundSystem.h
	OpenALSoundError.cpp
	OpenALSoundError.h	
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/openal-soft/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )

add_definitions(-DAL_LIBTYPE_STATIC)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

IF(MENGINE_TOOLCHAIN_MSVC OR MENGINE_TOOLCHAIN_MINGW)
    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}OpenAL32${MENGINE_LIB_SUFFIX}
        )
        
    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        Winmm
        )        
ENDIF()

IF(MENGINE_TOOLCHAIN_GRADLE)
    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}openal${MENGINE_SO_SUFFIX}
        )
ENDIF()

IF(MENGINE_TOOLCHAIN_MSVC)
    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        Winmm
        )
ENDIF()

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)
