SET( MY_LIB_NAME PackageService )

ADD_FILTER(
src
	PackageService.h
	PackageService.cpp
    Package.h
    Package.cpp
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
    ${THIRDPARTY_DIR}/metabuf/include
    ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
