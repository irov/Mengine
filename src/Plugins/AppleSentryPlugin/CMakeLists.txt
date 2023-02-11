MENGINE_PROJECT(AppleSentryPlugin)

ADD_FILTER(
src
    AppleSentryInterface.h

    AppleSentryPlugin.h
    AppleSentryPlugin.def
    AppleSentryPlugin.cpp

    AppleSentryLoggerCapture.h
    AppleSentryLoggerCapture.mm
    
    AppleSentryService.h
    AppleSentryService.mm
        
    AppleSentryHelper.h
    AppleSentryHelper.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SENTRY)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

ADD_MENGINE_COCOAPOD(Sentry "https://github.com/getsentry/sentry-cocoa.git" "8.1.0")