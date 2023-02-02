MENGINE_PROJECT(SentryPlugin)

ADD_FILTER(
src
    SentryInterface.h

    SentryPlugin.h
    SentryPlugin.def
    SentryPlugin.cpp
)

if(MENGINE_TARGET_WINDOWS)
    ADD_FILTER(
    win32
        Win32SentryLoggerCapture.h
        Win32SentryLoggerCapture.cpp
    
        Win32SentryService.h
        Win32SentryService.cpp
    )
endif()

if(MENGINE_TARGET_APPLE)
    ADD_FILTER(
    apple
        AppleSentryLoggerCapture.h
        AppleSentryLoggerCapture.cpp
    
        AppleSentryService.h
        AppleSentryService.mm
        
        AppleSentryHelper.h
        AppleSentryHelper.mm
    )
endif()

if(MENGINE_TARGET_WINDOWS)
    add_definitions(-DSENTRY_BUILD_STATIC)
    
    include_directories(${THIRDPARTY_DIR}/sentry/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SENTRY)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

if(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}sentry${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}mini_chromium${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crashpad_client${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crashpad_minidump${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crashpad_snapshot${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crashpad_tools${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crashpad_util${MENGINE_LIB_SUFFIX})

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Winhttp.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Rpcrt4.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Dbghelp.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Version.lib)
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_TARGET_APPLE)
    ADD_MENGINE_COCOAPOD(Sentry "https://github.com/getsentry/sentry-cocoa.git" "8.1.0")
endif()
