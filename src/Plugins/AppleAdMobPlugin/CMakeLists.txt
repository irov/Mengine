MENGINE_PROJECT(AppleAdMobPlugin)

ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_ADMOB BANNER OFF "MENGINE_PLUGIN_APPLE_ADMOB_BANNER")
ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_ADMOB INTERSTITIAL OFF "MENGINE_PLUGIN_APPLE_ADMOB_INTERSTITIAL")
ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_ADMOB REWARDED OFF "MENGINE_PLUGIN_APPLE_ADMOB_REWARDED")

ADD_FILTER(
src
    AppleAdMobPlugin.h
    AppleAdMobPlugin.mm

    AppleAdMobInterface.h
    
    AppleAdMobBaseDelegate.h
    AppleAdMobBaseDelegate.mm

    AppleAdMobInterstitialDelegate.h
    AppleAdMobInterstitialDelegate.mm

    AppleAdMobRewardedDelegate.h
    AppleAdMobRewardedDelegate.mm

    AppleAdMobBannerDelegate.h
    AppleAdMobBannerDelegate.mm
    
    AppleAdMobApplicationDelegate.h
    AppleAdMobApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_ADMOB)

ADD_MENGINE_COCOAPOD("GoogleMobileAds" "NO-GIT" "12.11.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAdMobApplicationDelegate")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleAdvertisementPlugin)

