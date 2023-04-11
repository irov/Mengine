MENGINE_PROJECT(AppleSentryPlugin)

ADD_FILTER(
src
    AppleSentryInterface.h

    AppleSentryPlugin.h
    AppleSentryPlugin.mm
    
    AppleSentryLoggerCapture.h
    AppleSentryLoggerCapture.mm
    
    AppleSentryService.h
    AppleSentryService.mm
        
    AppleSentryHelper.h
    AppleSentryHelper.mm
)

if(MENGINE_TARGET_IOS)
    ADD_FILTER(
    delegate
        AppleSentryApplicationDelegate.h
        AppleSentryApplicationDelegate.mm
    )
elseif(MENGINE_TARGET_MACOS)
    ADD_FILTER(
    delegate
        MacOSSentryApplicationDelegate.h
        MacOSSentryApplicationDelegate.mm
    )
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_SENTRY)

if(MENGINE_TARGET_IOS)
    ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleSentryApplicationDelegate")
elseif(MENGINE_TARGET_MACOS)
    ADD_MENGINE_APPLE_APPLICATION_DELEGATE("MacOSSentryApplicationDelegate")
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

ADD_MENGINE_COCOAPOD(Sentry "https://github.com/getsentry/sentry-cocoa.git" "8.4.0")
