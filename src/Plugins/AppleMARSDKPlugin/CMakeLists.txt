MENGINE_PROJECT(AppleMARSDKPlugin)

ADD_FILTER(
src
    AppleMARSDKInterface.h

    AppleMARSDKPlugin.h
    AppleMARSDKPlugin.mm

    AppleMARSDKApplicationDelegate.h
    AppleMARSDKApplicationDelegate.m

    AppleMARSDKDelegate.h
    AppleMARSDKDelegate.m

    AppleMARSDKCallbacks.h
    AppleMARSDKCallbacks.m
    
    AppleMARSDKService.h
    AppleMARSDKService.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_MARSDK)

ADD_APPLE_FRAMEWORK_WITH_PATH(MARSDKCore "${THIRDPARTY_DIR}/marsdk-ios")