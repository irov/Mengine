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

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "NO-TAG")
ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "12.1.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAppLovinApplicationDelegate")

macro(ADD_APPLOVIN_MEDIATION MEDIATION POD VERSION)
    ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_APPLOVIN MEDIATION_${MEDIATION} OFF "MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION}")
    if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION})
        ADD_MENGINE_COCOAPOD(${POD} "NO-GIT" ${VERSION})
    endif()
endmacro()

ADD_APPLOVIN_MEDIATION(ADCOLONY "AppLovinMediationAdColonyAdapter" "4.9.0.0.4")
ADD_APPLOVIN_MEDIATION(AMAZON "AppLovinMediationAmazonAdMarketplaceAdapter" "4.7.7.0")
ADD_APPLOVIN_MEDIATION(AMAZON "AmazonPublisherServicesSDK" "4.7.7")
ADD_APPLOVIN_MEDIATION(BIDMACHINE "AppLovinMediationBidMachineAdapter" "2.4.0.1.0")
ADD_APPLOVIN_MEDIATION(CHARTBOOST "AppLovinMediationChartboostAdapter" "9.5.1.0")
ADD_APPLOVIN_MEDIATION(CRITEO "AppLovinMediationCriteoAdapter" "5.0.4.0")
ADD_APPLOVIN_MEDIATION(CSJ "AppLovinMediationCSJAdapter" "4.9.0.7.2")
ADD_APPLOVIN_MEDIATION(DTEXCHANGE "AppLovinMediationFyberAdapter" "8.2.5.0")
ADD_APPLOVIN_MEDIATION(GOOGLEADMANAGER "AppLovinMediationGoogleAdManagerAdapter" "10.14.0.0")
ADD_APPLOVIN_MEDIATION(ADMOB "AppLovinMediationGoogleAdapter" "10.14.0.1")
ADD_APPLOVIN_MEDIATION(HYPRMX "AppLovinMediationHyprMXAdapter" "6.3.0.0")
ADD_APPLOVIN_MEDIATION(INMOBI "AppLovinMediationInMobiAdapter" "10.6.0.0")
ADD_APPLOVIN_MEDIATION(IRONSOURCE "AppLovinMediationIronSourceAdapter" "7.6.0.0.0")
ADD_APPLOVIN_MEDIATION(LIFTOFF "AppLovinMediationLifftoffAdapter" "7.2.0.0")
ADD_APPLOVIN_MEDIATION(LINE "AppLovinMediationLineAdapter" "2.7.20231115.0")
ADD_APPLOVIN_MEDIATION(MAIO "AppLovinMediationMaioAdapter" "2.1.1.0")
ADD_APPLOVIN_MEDIATION(META "AppLovinMediationFacebookAdapter" "6.14.0.1")
ADD_APPLOVIN_MEDIATION(MINTEGRAL "AppLovinMediationMintegralAdapter" "7.5.2.0.0")
ADD_APPLOVIN_MEDIATION(MOBILEFUSE "AppLovinMediationMobileFuseAdapter" "1.6.5.0")
ADD_APPLOVIN_MEDIATION(MYTARGET "AppLovinMediationMyTargetAdapter" "5.20.0.0")
ADD_APPLOVIN_MEDIATION(NEND "AppLovinMediationNendAdapter" "8.2.0.0")
ADD_APPLOVIN_MEDIATION(OGURY "AppLovinMediationOguryPresageAdapter" "4.2.2.0")
ADD_APPLOVIN_MEDIATION(PANGLE "AppLovinMediationByteDanceAdapter" "5.6.0.7.0")
ADD_APPLOVIN_MEDIATION(SMAATO "AppLovinMediationSmaatoAdapter" "22.5.0.0")
ADD_APPLOVIN_MEDIATION(TENCENT "AppLovinMediationTencentGDTAdapter" "4.14.50.0")
ADD_APPLOVIN_MEDIATION(UNITYADS "AppLovinMediationUnityAdsAdapter" "4.9.2.0")
ADD_APPLOVIN_MEDIATION(VERVE "AppLovinMediationVerveAdapter" "2.20.0.0")
ADD_APPLOVIN_MEDIATION(YANDEX "AppLovinMediationYandexAdapter" "6.1.0.3")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
