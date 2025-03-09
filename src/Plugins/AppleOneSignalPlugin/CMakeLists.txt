MENGINE_PROJECT(AppleOneSignalPlugin)

ADD_FILTER(
src
    AppleOneSignalPlugin.h
    AppleOneSignalPlugin.mm
    
    AppleOneSignalApplicationDelegate.h
    AppleOneSignalApplicationDelegate.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_ONESIGNAL)

ADD_MENGINE_COCOAPOD("OneSignalXCFramework" "NO-GIT" ">= 3.0.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleOneSignalApplicationDelegate")

MENGINE_PROJECT(AppleOneSignalNotificationServiceExtension)

ADD_FILTER(
src
    AppleOneSignalNotificationServiceExtension.h
    AppleOneSignalNotificationServiceExtension.mm
)

ADD_MENGINE_PLUGIN_MODULE()

set_target_properties(AppleOneSignalNotificationServiceExtension PROPERTIES
    BUNDLE YES
    XCODE_PRODUCT_TYPE com.apple.usernotifications.service)

ADD_MENGINE_COCOAPOD("OneSignalXCFramework" "NO-GIT" ">= 3.0.0")

