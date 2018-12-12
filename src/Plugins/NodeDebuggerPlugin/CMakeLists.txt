SET( MY_LIB_NAME NodeDebuggerPlugin )

ADD_FILTER(
    src
    NodeDebuggerInterface.h

    NodeDebuggerPlugin.h
    NodeDebuggerPlugin.def
    NodeDebuggerPlugin.cpp

    NodeDebuggerService.h
    NodeDebuggerService.cpp
	
	NodeDebuggerSerializaton.h
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pugixml/src )

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
