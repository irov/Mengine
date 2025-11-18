MENGINE_PROJECT(AppleUserMessagingPlatformPlugin)

ADD_FILTER(
src
    AppleUserMessagingPlatformInterface.h

    AppleUserMessagingPlatformApplicationDelegate.h
    AppleUserMessagingPlatformApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_USERMESSAGINGPLATFORM)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleUserMessagingPlatformApplicationDelegate")

ADD_MENGINE_COCOAPOD("GoogleUserMessagingPlatform" "NO-GIT" "3.1.0")


