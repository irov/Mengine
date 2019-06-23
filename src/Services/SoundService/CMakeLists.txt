SET( MY_LIB_NAME SoundService )

ADD_FILTER(
src
    SoundIdentity.h
    SoundIdentity.cpp
	SoundService.h
	SoundService.cpp
    ThreadWorkerSoundBufferUpdate.h
    ThreadWorkerSoundBufferUpdate.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
)

INCLUDE_DIRECTORIES( 
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
