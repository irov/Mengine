MENGINE_PROJECT(AppleUserNotificationCenterPlugin)

ADD_FILTER(
src
    AppleUserNotificationCenterInterface.h

    AppleUserNotificationCenterPlugin.h
    AppleUserNotificationCenterPlugin.mm
    
    AppleUserNotificationCenterApplicationDelegate.h
    AppleUserNotificationCenterApplicationDelegate.mm
    
    AppleUserNotificationCenterService.h
    AppleUserNotificationCenterService.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_USERNOTIFICATIONCENTER)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleUserNotificationCenterApplicationDelegate")