SET( MY_LIB_NAME ServiceConfig )

ADD_FILTER(
src
	ConfigManager.h
	ConfigManager.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/libmath/include
    ${THIRDPARTY_DIR}/tinyini/include
    ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
