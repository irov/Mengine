MENGINE_PROJECT(iOSUserNotificationCenterPlugin)

ADD_FILTER(
src
    iOSUserNotificationCenterPlugin.h
    iOSUserNotificationCenterPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_USERNOTIFICATIONCENTER)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSUserNotificationCenterPlugin")