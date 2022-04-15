MENGINE_PROJECT(AppleAppLovinPlugin)

ADD_FILTER(
src
    AppleAppLovinPlugin.h
    AppleAppLovinPlugin.cpp

    AppleAppLovinInterface.h

    AppleAppLovinInterstitialDelegate.h
    AppleAppLovinInterstitialDelegate.m

    AppleAppLovinRewardedDelegate.h
    AppleAppLovinRewardedDelegate.mm

    AppleAppLovinBannerDelegate.h
    AppleAppLovinBannerDelegate.m

    AppleAppLovinService.h
    AppleAppLovinService.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPLOVIN)

ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "11.3.3")