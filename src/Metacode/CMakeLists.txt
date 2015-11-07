set( MY_LIB_NAME Metacode )

ADD_FILTER(
Metabuf
	Metacode.cpp
	Metacode.h
    Metatype.cpp
    Metatype.h
    protocol.xml
)

ADD_FILTER(
Loader
	LoaderEngine.cpp
	LoaderEngine.h
)
   
INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/boost
    )     

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Core
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}metabuf_static${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)