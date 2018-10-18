SET( MY_LIB_NAME ServiceSound )

ADD_FILTER(
src
    SoundIdentity.h
    SoundIdentity.cpp
	SoundEngine.h
	SoundEngine.cpp
    ThreadWorkerSoundBufferUpdate.h
    ThreadWorkerSoundBufferUpdate.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/pybind/include
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
