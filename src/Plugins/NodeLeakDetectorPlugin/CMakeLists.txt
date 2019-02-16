SET(MY_LIB_NAME NodeLeakDetectorPlugin)

ADD_FILTER(
src
    NodeLeakDetectorPlugin.h
    NodeLeakDetectorPlugin.cpp
    NodeLeakDetectorPlugin.def
    NodeLeakDetectorServiceInterface.h
    NodeLeakDetectorService.h
    NodeLeakDetectorService.cpp
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)