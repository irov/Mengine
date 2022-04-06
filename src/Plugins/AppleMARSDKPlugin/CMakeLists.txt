MENGINE_PROJECT(AppleMARSDKPlugin)

ADD_FILTER(
src
    AppleMARSDKInterface.h

    AppleMARSDKPlugin.h
    AppleMARSDKPlugin.mm

    AppleMARSDKApplicationDelegate.h
    AppleMARSDKApplicationDelegate.m
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_MARSDK)

ADD_APPLE_FRAMEWORK_WITH_PATH(MARSDKCore "${THIRDPARTY_DIR}/marsdk-ios")