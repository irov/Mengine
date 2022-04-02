MENGINE_PROJECT(DevToDebugPlugin)

ADD_FILTER(
src
    DevToDebugInterface.h

    DevToDebugPlugin.h
    DevToDebugPlugin.def
    DevToDebugPlugin.cpp
    
    DevToDebugService.h
    DevToDebugService.cpp
    
    DevToDebugProvider.h
    DevToDebugProvider.cpp

    DevToDebugWidget.h
    DevToDebugWidget.cpp
    
    DevToDebugWidgetButton.h
    DevToDebugWidgetButton.cpp
    
    DevToDebugWidgetText.h
    DevToDebugWidgetText.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEVTODEBUG)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)