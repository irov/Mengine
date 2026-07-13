MENGINE_PROJECT(MacOSSentryPlugin)

ADD_FILTER(
src
    MacOSSentryNotificationObserver.h
    MacOSSentryNotificationObserver.mm

    MacOSSentryPlugin.h
    MacOSSentryPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MACOS_SENTRY)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("MacOSSentryPlugin")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

ADD_MENGINE_COCOAPOD(Sentry "https://github.com/getsentry/sentry-cocoa.git" "9.19.1")
