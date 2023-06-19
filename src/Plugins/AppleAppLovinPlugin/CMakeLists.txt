MENGINE_PROJECT(AppleAppLovinPlugin)

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

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPLOVIN)

ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "11.10.1")

macro(ADD_APPLOVIN_MEDIATION MEDIATION POD VERSION)
    ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_APPLOVIN MEDIATION_${MEDIATION} OFF "MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION}")
    if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION})
        ADD_MENGINE_COCOAPOD(${POD} "NO-GIT" ${VERSION})
    endif()
endmacro()

ADD_APPLOVIN_MEDIATION(ADCOLONY "AppLovinMediationAdColonyAdapter" "4.9.0.0.4")
ADD_APPLOVIN_MEDIATION(AMAZON "AppLovinMediationAmazonAdMarketplaceAdapter" "4.7.2.0")
ADD_APPLOVIN_MEDIATION(AMAZON "AmazonPublisherServicesSDK" "4.7.2")
ADD_APPLOVIN_MEDIATION(BIDMACHINE "AppLovinMediationBidMachineAdapter" "2.1.0.0.1")
ADD_APPLOVIN_MEDIATION(CHARTBOOST "AppLovinMediationChartboostAdapter" "9.3.0.1")
ADD_APPLOVIN_MEDIATION(CRITEO "AppLovinMediationCriteoAdapter" "4.9.0.0")
ADD_APPLOVIN_MEDIATION(DTEXCHANGE "AppLovinMediationFyberAdapter" "8.2.2.0")
ADD_APPLOVIN_MEDIATION(ADMOB "AppLovinMediationGoogleAdapter" "10.6.0.0")
ADD_APPLOVIN_MEDIATION(HYPRMX "AppLovinMediationHyprMXAdapter" "6.2.0.1")
ADD_APPLOVIN_MEDIATION(INMOBI "AppLovinMediationInMobiAdapter" "10.1.4.1")
ADD_APPLOVIN_MEDIATION(IRONSOURCE "AppLovinMediationIronSourceAdapter" "7.3.1.1")
ADD_APPLOVIN_MEDIATION(LIFTOFF "AppLovinMediationLifftoffAdapter" "6.12.3.0")
ADD_APPLOVIN_MEDIATION(LINE "AppLovinMediationLineAdapter" "2.6.20230609.0")
ADD_APPLOVIN_MEDIATION(MAIO "AppLovinMediationMaioAdapter" "1.6.3.1")
ADD_APPLOVIN_MEDIATION(META "AppLovinMediationFacebookAdapter" "6.12.0.3")
ADD_APPLOVIN_MEDIATION(MINTEGRAL "AppLovinMediationMintegralAdapter" "7.3.8.0.0")
ADD_APPLOVIN_MEDIATION(MOBILEFUSE "AppLovinMediationMobileFuseAdapter" "1.5.2.0")
ADD_APPLOVIN_MEDIATION(MYTARGET "AppLovinMediationMyTargetAdapter" "5.17.5.0")
ADD_APPLOVIN_MEDIATION(NEND "AppLovinMediationNendAdapter" "8.0.1.0")
ADD_APPLOVIN_MEDIATION(OGURY "AppLovinMediationOguryPresageAdapter" "4.1.2.2")
ADD_APPLOVIN_MEDIATION(PANGLE "AppLovinMediationByteDanceAdapter" "5.2.1.1.0")
ADD_APPLOVIN_MEDIATION(SMAATO "AppLovinMediationSmaatoAdapter" "22.1.2.0")
ADD_APPLOVIN_MEDIATION(TAPJOY "AppLovinMediationTapjoyAdapter" "13.0.1.0")
ADD_APPLOVIN_MEDIATION(TENCENT "AppLovinMediationTencentGDTAdapter" "4.14.30.0")
ADD_APPLOVIN_MEDIATION(UNITYADS "AppLovinMediationUnityAdsAdapter" "4.7.1.0")
ADD_APPLOVIN_MEDIATION(VERVE "AppLovinMediationVerveAdapter" "2.18.1.1")
ADD_APPLOVIN_MEDIATION(YANDEX "AppLovinMediationYandexAdapter" "5.7.0.0")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
