MENGINE_PROJECT(DevToDebugPlugin)

ADD_FILTER(
src
    DevToDebugInterface.h

    DevToDebugPlugin.h
    DevToDebugPlugin.def
    DevToDebugPlugin.cpp
    
    DevToDebugService.h
    DevToDebugService.cpp
    
    DevToDebugTab.h
    DevToDebugTab.cpp
    
    DevToDebugProperty.h
    DevToDebugProperty.cpp
    
    DevToDebugPropertyConstBoolean.h
    DevToDebugPropertyConstBoolean.cpp

    DevToDebugPropertyGetterBoolean.h
    DevToDebugPropertyGetterBoolean.cpp
    
    DevToDebugPropertyInitialBoolean.h
    DevToDebugPropertyInitialBoolean.cpp
    
    DevToDebugPropertyConstString.h
    DevToDebugPropertyConstString.cpp
    
    DevToDebugPropertyGetterString.h
    DevToDebugPropertyGetterString.cpp
    
    DevToDebugPropertyConstColor.h
    DevToDebugPropertyConstColor.cpp
    
    DevToDebugPropertyGetterColor.h
    DevToDebugPropertyGetterColor.cpp
    
    DevToDebugWidget.h
    DevToDebugWidget.cpp
    
    DevToDebugWidgetButton.h
    DevToDebugWidgetButton.cpp
    
    DevToDebugWidgetText.h
    DevToDebugWidgetText.cpp
    
    DevToDebugWidgetCheckbox.h
    DevToDebugWidgetCheckbox.cpp
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        DevToDebugScriptEmbedding.cpp
        DevToDebugScriptEmbedding.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEVTODEBUG)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()