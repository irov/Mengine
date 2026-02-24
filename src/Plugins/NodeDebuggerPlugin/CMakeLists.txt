MENGINE_PROJECT(NodeDebuggerPlugin)

ADD_FILTER(
    src
    NodeDebuggerPlugin.h
    NodeDebuggerPlugin.def
    NodeDebuggerPlugin.cpp

    NodeDebuggerModule.h
    NodeDebuggerModule.cpp

    NodeDebuggerSerialization.h

    SceneDataProviderInterface.h
)

ADD_FILTER(
    DebuggerBoundingBox
    HotSpotPolygonDebuggerBoundingBox.h
    HotSpotPolygonDebuggerBoundingBox.cpp
    TextFieldDebuggerBoundingBox.h
    TextFieldDebuggerBoundingBox.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pugixml/src)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_NODEDEBUGGER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pugixml${MENGINE_LIB_SUFFIX})

if(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zsd${MENGINE_LIB_SUFFIX})
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}z${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_UWP)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)
ENDIF()