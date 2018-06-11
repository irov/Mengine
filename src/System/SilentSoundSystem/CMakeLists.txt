set( MY_LIB_NAME SilentSoundSystem )

ADD_FILTER(
src
	SilentSoundBuffer.cpp
	SilentSoundBuffer.h
	SilentSoundSource.cpp
	SilentSoundSource.h
	SilentSoundSystem.cpp
	SilentSoundSystem.h
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}
    ${THIRDPARTY_DIR}/libmath/include
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
