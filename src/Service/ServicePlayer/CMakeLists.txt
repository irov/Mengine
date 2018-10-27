SET( MY_LIB_NAME ServicePlayer )

ADD_FILTER(
src
	PlayerService.h
	PlayerService.cpp    
    GlobalAffectorable.h
    GlobalAffectorable.cpp
    GlobalHandleSystem.h
    GlobalHandleSystem.cpp
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
