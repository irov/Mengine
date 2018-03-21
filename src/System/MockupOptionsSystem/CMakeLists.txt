SET( MY_LIB_NAME MockupOptionsSystem )

ADD_FILTER(
src
	MockupOptionsSystem.h
	MockupOptionsSystem.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}
    ${Mengine_SOURCE_DIR}/Utils
    ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
