set( MY_LIB_NAME ResourceValidatePlugin )

ADD_FILTER(
src
    ResourceValidateInterface.h

	ResourceValidatePlugin.h
    ResourceValidatePlugin.def
	ResourceValidatePlugin.cpp
    
    ResourceValidateService.h
    ResourceValidateService.cpp
    
    ResourceValidateVisitor.h
    ResourceValidateVisitor.cpp
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
        ${THIRDPARTY_DIR}/libmath/include
        ${THIRDPARTY_DIR}/pybind/include
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