set( MY_LIB_NAME XcodeConsoleLogger )

ADD_FILTER(
src
	XcodeConsoleLoggerPlugin.h
	XcodeConsoleLoggerPlugin.cpp
    
    XcodeConsoleLogger.h
    XcodeConsoleLogger.cpp
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
)

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
