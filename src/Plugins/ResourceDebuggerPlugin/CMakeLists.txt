SET(MY_LIB_NAME ResourceDebuggerPlugin)

ADD_FILTER(
src
    ResourceDebuggerPlugin.h
    ResourceDebuggerPlugin.cpp
    ResourceDebuggerPlugin.def
    ResourceUselessCompileChecker.h
    ResourceUselessCompileChecker.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_RESOURCEDEBUGGER)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)