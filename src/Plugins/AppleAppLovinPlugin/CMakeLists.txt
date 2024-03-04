MENGINE_PROJECT(AppleAppLovinPlugin)

ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_APPLOVIN CONSENT_FLOW OFF "MENGINE_PLUGIN_APPLE_APPLOVIN_CONSENT_FLOW")

ADD_FILTER(
src
    AppleAppLovinPlugin.h
    AppleAppLovinPlugin.mm

    AppleAppLovinInterface.h
    
    AppleAppLovinBaseDelegate.h
    AppleAppLovinBaseDelegate.mm

    AppleAppLovinInterstitialDelegate.h
    AppleAppLovinInterstitialDelegate.mm

    AppleAppLovinRewardedDelegate.h
    AppleAppLovinRewardedDelegate.mm

    AppleAppLovinBannerDelegate.h
    AppleAppLovinBannerDelegate.mm

    AppleAppLovinService.h
    AppleAppLovinService.mm

    AppleAppLovinAnalyticsDelegate.h

    AppleAppLovinAnalyticsService.h
    AppleAppLovinAnalyticsService.mm
    
    AppleAppLovinApplicationDelegate.h
    AppleAppLovinApplicationDelegate.mm
)

if(MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS)
    ADD_FILTER(
        firebase
        AppleAppLovinFirebaseAnalytics.h
        AppleAppLovinFirebaseAnalytics.mm
    )
endif()

if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    ADD_FILTER(
    amazon
        AppleAppLovinAmazonService.h
        AppleAppLovinAmazonService.mm
        
        AppleAppLovinInterstitialAmazonLoader.h
        AppleAppLovinInterstitialAmazonLoader.mm
        
        AppleAppLovinRewardedAmazonLoader.h
        AppleAppLovinRewardedAmazonLoader.mm
        
        AppleAppLovinBannerAmazonLoader.h
        AppleAppLovinBannerAmazonLoader.mm
    )
endif()

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAppLovinScriptEmbedding.h
        AppleAppLovinScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_APPLOVIN)

if(MENGINE_PLUGIN_APPLE_APPLOVIN_CONSENT_FLOW)
    ADD_MENGINE_COCOAPOD("GoogleUserMessagingPlatform" "NO-GIT" "2.2.0")
endif()

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "7.13.0")
ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "12.2.1")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAppLovinApplicationDelegate")

macro(ADD_APPLOVIN_MEDIATION MEDIATION POD VERSION)
    ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_APPLOVIN MEDIATION_${MEDIATION} OFF "MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION}")
    if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION})
        ADD_MENGINE_COCOAPOD(${POD} "NO-GIT" ${VERSION})
    endif()
endmacro()

ADD_APPLOVIN_MEDIATION(AMAZON "AppLovinMediationAmazonAdMarketplaceAdapter" "4.7.8.0")
ADD_APPLOVIN_MEDIATION(AMAZON "AmazonPublisherServicesSDK" "4.7.8")
ADD_APPLOVIN_MEDIATION(BIDMACHINE "AppLovinMediationBidMachineAdapter" "2.5.1.0.0")
ADD_APPLOVIN_MEDIATION(CHARTBOOST "AppLovinMediationChartboostAdapter" "9.6.0.0")
ADD_APPLOVIN_MEDIATION(CSJ "AppLovinMediationCSJAdapter" "4.9.0.7.2")
ADD_APPLOVIN_MEDIATION(DTEXCHANGE "AppLovinMediationFyberAdapter" "8.2.5.0")
ADD_APPLOVIN_MEDIATION(GOOGLEADMANAGER "AppLovinMediationGoogleAdManagerAdapter" "11.0.1.0")
ADD_APPLOVIN_MEDIATION(ADMOB "AppLovinMediationGoogleAdapter" "11.0.1.0")
ADD_APPLOVIN_MEDIATION(HYPRMX "AppLovinMediationHyprMXAdapter" "6.3.0.1")
ADD_APPLOVIN_MEDIATION(INMOBI "AppLovinMediationInMobiAdapter" "10.6.4.0")
ADD_APPLOVIN_MEDIATION(IRONSOURCE "AppLovinMediationIronSourceAdapter" "7.8.0.0.0")
ADD_APPLOVIN_MEDIATION(LIFTOFF "AppLovinMediationLifftoffAdapter" "7.2.2.0")
ADD_APPLOVIN_MEDIATION(LINE "AppLovinMediationLineAdapter" "2.7.20240214.0")
ADD_APPLOVIN_MEDIATION(MAIO "AppLovinMediationMaioAdapter" "2.1.2.0")
ADD_APPLOVIN_MEDIATION(META "AppLovinMediationFacebookAdapter" "6.14.0.1")
ADD_APPLOVIN_MEDIATION(MINTEGRAL "AppLovinMediationMintegralAdapter" "7.5.7.0.0")
ADD_APPLOVIN_MEDIATION(MOBILEFUSE "AppLovinMediationMobileFuseAdapter" "1.7.1.0")
ADD_APPLOVIN_MEDIATION(MYTARGET "AppLovinMediationMyTargetAdapter" "5.20.1.1")
ADD_APPLOVIN_MEDIATION(NEND "AppLovinMediationNendAdapter" "8.2.0.0")
ADD_APPLOVIN_MEDIATION(OGURY "AppLovinMediationOguryPresageAdapter" "4.2.2.1")
ADD_APPLOVIN_MEDIATION(PANGLE "AppLovinMediationByteDanceAdapter" "5.8.0.7.0")
ADD_APPLOVIN_MEDIATION(SMAATO "AppLovinMediationSmaatoAdapter" "22.7.0.0")
ADD_APPLOVIN_MEDIATION(TENCENT "AppLovinMediationTencentGDTAdapter" "4.14.63.0")
ADD_APPLOVIN_MEDIATION(UNITYADS "AppLovinMediationUnityAdsAdapter" "4.9.3.0")
ADD_APPLOVIN_MEDIATION(VERVE "AppLovinMediationVerveAdapter" "2.21.1.0")
ADD_APPLOVIN_MEDIATION(YANDEX "AppLovinMediationYandexAdapter" "6.1.0.3")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
