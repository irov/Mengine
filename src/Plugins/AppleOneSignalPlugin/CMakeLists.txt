MENGINE_PROJECT(AppleOneSignalPlugin)

ADD_FILTER(
src
    AppleOneSignalPlugin.h
    AppleOneSignalPlugin.cpp

    AppleOneSignalInterface.h

    AppleOneSignalService.h
    AppleOneSignalService.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_ONESIGNAL)

ADD_MENGINE_COCOAPOD("OneSignalXCFramework" "NO-GIT" ">= 3.0.0")

MENGINE_PROJECT(AppleOneSignalNotificationServiceExtension)

set_target_properties(AppleOneSignalNotificationServiceExtension PROPERTIES
    BUNDLE YES
    XCODE_PRODUCT_TYPE com.apple.usernotifications.service)
    
ADD_FILTER(
src
    AppleOneSignalNotificationServiceExtension.h
    AppleOneSignalNotificationServiceExtension.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_ONESIGNAL)

ADD_MENGINE_COCOAPOD("OneSignalXCFramework" "NO-GIT" ">= 3.0.0")

