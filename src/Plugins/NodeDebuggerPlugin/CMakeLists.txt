SET(MY_LIB_NAME NodeDebuggerPlugin)

ADD_FILTER(
    src
    NodeDebuggerPlugin.h
    NodeDebuggerPlugin.def
    NodeDebuggerPlugin.cpp

    NodeDebuggerModule.h
    NodeDebuggerModule.cpp
	
	NodeDebuggerSerialization.h
    
    NodeDebuggerBoundingBoxInterface.h
    HotSpotPolygonDebuggerBoundingBox.h
    HotSpotPolygonDebuggerBoundingBox.cpp
    TextFieldDebuggerBoundingBox.h
    TextFieldDebuggerBoundingBox.cpp
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pugixml/src )

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_NODEDEBUGGER)

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pugixml${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)