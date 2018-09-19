set( MY_LIB_NAME MoviePlugin )

ADD_FILTER(
src
    Movie2Interface.h
    
	MoviePlugin.h
	MoviePlugin.cpp
    
    Movie2.cpp
    Movie2.h
    Movie2Slot.cpp
    Movie2Slot.h
    Movie2Scissor.cpp
    Movie2Scissor.h
    Movie2SubComposition.cpp
    Movie2SubComposition.h
    ResourceMovie2.cpp
    ResourceMovie2.h
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
        ${THIRDPARTY_DIR}/libmovie/include
        ${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/libmath/include
        ${THIRDPARTY_DIR}/metabuf
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Engine
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Kernel
    Engine
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}movie${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)