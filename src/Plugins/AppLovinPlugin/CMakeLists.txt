MENGINE_PROJECT(AppLovinPlugin)

ADD_FILTER(
src
    # SentryInterface.h

    AppLovinPlugin.h
    AppLovinPlugin.cpp
)


if(MENGINE_TARGET_IOS)
    ADD_FILTER(
    apple

        AppLovinApplicationDelegate.h
        AppLovinApplicationDelegate.m

        AppLovinInterstitialDelegate.h
        AppLovinInterstitialDelegate.m

        AppLovinRewardedDelegate.h
        AppLovinRewardedDelegate.m

        AppLovinBannerDelegate.h
        AppLovinBannerDelegate.m

        AppLovinService.h
        AppLovinService.mm
    )
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLOVIN)

if(MENGINE_TARGET_IOS)
    ADD_MENGINE_COCOAPOD("AppLovinSDK" "11.3.3")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationAdColonyAdapter" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationAmazonAdMarketplaceAdapter" " ")
    # ADD_MENGINE_COCOAPOD("AmazonPublisherServicesSDK" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationChartboostAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationCriteoAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationFyberAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationGoogleAdManagerAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationGoogleAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationHyprMXAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationInMobiAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationIronSourceAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationLineAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationMaioAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationFacebookAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationMintegralAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationMyTargetAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationNendAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationOguryPresageAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationByteDanceAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationSmaatoAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationSnapAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationTapjoyAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationTencentGDTAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationUnityAdsAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationVerveAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationVungleAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationVerizonAdsAdapter" "")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationYandexAdapter" "")
    
    set_xcode_property(${PROJECT_NAME} ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES[variant=Debug] "YES")
    set_xcode_property(${PROJECT_NAME} ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES[variant=Release] "YES")
endif()