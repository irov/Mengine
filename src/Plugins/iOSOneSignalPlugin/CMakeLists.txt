MENGINE_PROJECT(iOSOneSignalPlugin)

ADD_FILTER(
src
    iOSOneSignalPlugin.h
    iOSOneSignalPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_ONESIGNAL)

ADD_MENGINE_COCOAPOD("OneSignalXCFramework" "NO-GIT" ">= 3.0.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSOneSignalPlugin")

MENGINE_PROJECT(iOSOneSignalNotificationServiceExtension)

ADD_FILTER(
src
    iOSOneSignalNotificationServiceExtension.h
    iOSOneSignalNotificationServiceExtension.mm
)

ADD_MENGINE_PLUGIN_MODULE()

set_target_properties(iOSOneSignalNotificationServiceExtension PROPERTIES
    BUNDLE YES
    XCODE_PRODUCT_TYPE com.apple.usernotifications.service)

ADD_MENGINE_COCOAPOD("OneSignalXCFramework" "NO-GIT" ">= 3.0.0")
