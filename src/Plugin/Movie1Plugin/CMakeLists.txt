set( MY_LIB_NAME Movie1Plugin )

ADD_FILTER(
src
    Movie1Interface.h
    
	Movie1Plugin.h
	Movie1Plugin.cpp
    
    Movie.h
    Movie.cpp
    MovieSlot.h
    MovieSlot.cpp    
    MovieSceneEffect.h
    MovieSceneEffect.cpp
    MovieNodeExtra.h
    MovieNodeExtra.cpp
    MovieInternalObject.h
    MovieInternalObject.cpp
    MovieEvent.h
    MovieEvent.cpp
    ResourceMovie.cpp
    ResourceMovie.h
    ResourceInternalObject.cpp
    ResourceInternalObject.h
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
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
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)