SET(MY_LIB_NAME ResourceDebuggerPlugin)

ADD_FILTER(
src
    ResourceDebuggerPlugin.h
    ResourceDebuggerPlugin.cpp
    ResourceDebuggerPlugin.def
    ResourceUselessCompileChecker.h
    ResourceUselessCompileChecker.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)