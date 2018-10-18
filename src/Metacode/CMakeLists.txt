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
    ${THIRDPARTY_DIR}/libmath/include
    ${THIRDPARTY_DIR}/metabuf
    )     

ADD_MENGINE_LIBRARY()

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)