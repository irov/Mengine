set( MY_LIB_NAME DebugRenderPlugin )

ADD_FILTER(
src
    DebugRenderInterface.h

	DebugRenderPlugin.h
    DebugRenderPlugin.def
	DebugRenderPlugin.cpp
    
    DebugRenderService.h
    DebugRenderService.cpp
    
    NodeDebugRenderVisitor.h
    NodeDebugRenderVisitor.cpp
)

INCLUDE_DIRECTORIES( 
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