SET( MY_LIB_NAME GameService )

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
    FOLDER Services
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
