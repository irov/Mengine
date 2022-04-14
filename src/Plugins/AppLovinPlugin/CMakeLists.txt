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

        AppLovinInterface.h

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
    ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "11.3.3")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationAdColonyAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationAmazonAdMarketplaceAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AmazonPublisherServicesSDK" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationChartboostAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationCriteoAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationFyberAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationGoogleAdManagerAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationGoogleAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationHyprMXAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationInMobiAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationIronSourceAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationLineAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationMaioAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationFacebookAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationMintegralAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationMyTargetAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationNendAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationOguryPresageAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationByteDanceAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationSmaatoAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationSnapAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationTapjoyAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationTencentGDTAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationUnityAdsAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationVerveAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationVungleAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationVerizonAdsAdapter" "NO-GIT" " ")
    # ADD_MENGINE_COCOAPOD("AppLovinMediationYandexAdapter" "NO-GIT" " ")
    
    set_xcode_property(${PROJECT_NAME} ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES[variant=Debug] "YES")
    set_xcode_property(${PROJECT_NAME} ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES[variant=Release] "YES")
endif()