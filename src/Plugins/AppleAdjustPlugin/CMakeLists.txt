MENGINE_PROJECT(AppleAdjust)

ADD_FILTER(
src
    AppleAdjustPlugin.h
    AppleAdjustPlugin.cpp

    AppleAdjustInterface.h

    AppleAdjustService.h
    AppleAdjustService.mm

    AppleAdjustApplicationDelegate.h
    AppleAdjustApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_ADJUST)

ADD_MENGINE_COCOAPOD("Adjust" "https://github.com/adjust/ios_sdk.git" "v4.29.7")
ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAdjustApplicationDelegate")

IF(MENGINE_TARGET_IOS)
    ADD_APPLE_FRAMEWORK(AdSupport)
    ADD_APPLE_FRAMEWORK(iAd)
    ADD_APPLE_FRAMEWORK(AdServices)
    ADD_APPLE_FRAMEWORK(CoreTelephony)
    ADD_APPLE_FRAMEWORK(StoreKit)
    ADD_APPLE_FRAMEWORK(AppTrackingTransparency)
    ADD_APPLE_FRAMEWORK(WebKit)
ENDIF()