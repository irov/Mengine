SET( MY_LIB_NAME ServiceGraveyard )

ADD_FILTER(
src
	GraveyardService.cpp
	GraveyardService.h
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
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
