MENGINE_PROJECT(iOSOneSignalPlugin)

ADD_FILTER(
src
    iOSOneSignalPlugin.h
    iOSOneSignalPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_ONESIGNAL)

ADD_MENGINE_COCOAPOD("OneSignal/OneSignal" "NO-GIT" "5.5.6")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSOneSignalPlugin")

MENGINE_PROJECT(iOSOneSignalNotificationServiceExtension)

ADD_FILTER(
src
    iOSOneSignalNotificationServiceExtension.h
    iOSOneSignalNotificationServiceExtension.mm
    iOSOneSignalNotificationServiceExtensionInfo.plist.in
)

ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES})

set_target_properties(iOSOneSignalNotificationServiceExtension PROPERTIES
    BUNDLE_EXTENSION appex
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/iOSOneSignalNotificationServiceExtensionInfo.plist.in"
    XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK YES
    XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "$(inherited)"
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION "${MENGINE_BUILD_NUMBER}"
    XCODE_ATTRIBUTE_MARKETING_VERSION "${MENGINE_BUILD_VERSION}"
    XCODE_ATTRIBUTE_SKIP_INSTALL YES
    XCODE_EXPLICIT_FILE_TYPE wrapper.app-extension
    XCODE_PRODUCT_TYPE com.apple.product-type.app-extension)

ADD_MENGINE_APPLE_APP_EXTENSION(iOSOneSignalNotificationServiceExtension OneSignalNotificationServiceExtension)
ADD_MENGINE_APPLE_FRAMEWORK(UserNotifications)

ADD_MENGINE_COCOAPOD("OneSignal/OneSignal" "NO-GIT" "5.5.6")
