SET( MY_LIB_NAME ServiceGame )

ADD_FILTER(
src
	GameService.h
	GameService.cpp
    GameServiceAccountProvider.cpp
    GameServiceAccountProvider.h
    GameServiceSoundVolumeProvider.cpp
    GameServiceSoundVolumeProvider.h
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
