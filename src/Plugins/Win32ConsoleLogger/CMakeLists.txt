set(MY_LIB_NAME Win32ConsoleLogger)

ADD_FILTER(
src
	Win32ConsoleLoggerPlugin.h
	Win32ConsoleLoggerPlugin.cpp
    
    Win32ConsoleLogger.h
    Win32ConsoleLogger.cpp
)

add_definitions(-DMENGINE_PLUGIN_DLL)

ADD_MENGINE_SHARED(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
