MENGINE_PROJECT(AppleUserNotificationCenterPlugin)

ADD_FILTER(
src    
    AppleUserNotificationCenterApplicationDelegate.h
    AppleUserNotificationCenterApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_USERNOTIFICATIONCENTER)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleUserNotificationCenterApplicationDelegate")