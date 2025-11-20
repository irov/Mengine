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

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AppleAdMobScriptEmbedding.h
        AppleAdMobScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_ADMOB)

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("Google-Mobile-Ads-SDK" "NO-GIT" "12.13.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAdMobApplicationDelegate")

ADD_MENGINE_SKADNETWORKITEMS_PLIST_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SKAdNetworkItems.plist")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleAdvertisementPlugin)

