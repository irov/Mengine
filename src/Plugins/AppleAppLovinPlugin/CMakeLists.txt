MENGINE_PROJECT(AppleAppLovinPlugin)

ADD_FILTER(
src
    AppleAppLovinPlugin.h
    AppleAppLovinPlugin.cpp

    AppleAppLovinInterface.h

    AppleAppLovinInterstitialDelegate.h
    AppleAppLovinInterstitialDelegate.mm

    AppleAppLovinRewardedDelegate.h
    AppleAppLovinRewardedDelegate.mm

    AppleAppLovinBannerDelegate.h
    AppleAppLovinBannerDelegate.mm

    AppleAppLovinService.h
    AppleAppLovinService.mm
)

ADD_FILTER(
amazon
    AppleAppLovinInterstitialAmazonLoader.h
    AppleAppLovinInterstitialAmazonLoader.mm
    
    AppleAppLovinRewardedAmazonLoader.h
    AppleAppLovinRewardedAmazonLoader.mm
    
    AppleAppLovinBannerAmazonLoader.h
    AppleAppLovinBannerAmazonLoader.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAppLovinScriptEmbedding.h
        AppleAppLovinScriptEmbedding.cpp
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPLOVIN)

ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "11.3.3")
ADD_MENGINE_COCOAPOD("AmazonPublisherServicesSDK" "NO-GIT" "4.5.2")
ADD_MENGINE_COCOAPOD("AppLovinMediationAmazonAdMarketplaceAdapter" "NO-GIT" "4.5.2.1")


if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
