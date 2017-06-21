SET( MY_LIB_NAME ServicePackage )

ADD_FILTER(
src
	PackageService.h
	PackageService.cpp
    Package.h
    Package.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/metabuf
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
