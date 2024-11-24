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
    
    AppleAppLovinApplicationDelegate.h
    AppleAppLovinApplicationDelegate.mm
)

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
    ADD_MENGINE_COCOAPOD("GoogleUserMessagingPlatform" "NO-GIT" "2.7.0")
endif()

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "13.0.1")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAppLovinApplicationDelegate")

macro(ADD_APPLOVIN_MEDIATION MEDIATION POD VERSION)
    ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_APPLOVIN MEDIATION_${MEDIATION} OFF "MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION}")
    if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION})
        ADD_MENGINE_COCOAPOD(${POD} "NO-GIT" ${VERSION})
    endif()
endmacro()

ADD_APPLOVIN_MEDIATION(ADMOB "AppLovinMediationGoogleAdapter" "11.12.0.0")
ADD_APPLOVIN_MEDIATION(AMAZON "AppLovinMediationAmazonAdMarketplaceAdapter" "4.10.1.0")
ADD_APPLOVIN_MEDIATION(AMAZON "AmazonPublisherServicesSDK" "4.10.1")
ADD_APPLOVIN_MEDIATION(BIDMACHINE "AppLovinMediationBidMachineAdapter" "3.1.1.0.0")
ADD_APPLOVIN_MEDIATION(CHARTBOOST "AppLovinMediationChartboostAdapter" "9.8.0.0")
ADD_APPLOVIN_MEDIATION(DTEXCHANGE "AppLovinMediationFyberAdapter" "8.3.3.1")
ADD_APPLOVIN_MEDIATION(GOOGLEADMANAGER "AppLovinMediationGoogleAdManagerAdapter" "11.12.0.0")
ADD_APPLOVIN_MEDIATION(HYPRMX "AppLovinMediationHyprMXAdapter" "6.4.1.0.2")
ADD_APPLOVIN_MEDIATION(INMOBI "AppLovinMediationInMobiAdapter" "10.8.0.0")
ADD_APPLOVIN_MEDIATION(IRONSOURCE "AppLovinMediationIronSourceAdapter" "8.5.0.0.0")
ADD_APPLOVIN_MEDIATION(LIFTOFF "AppLovinMediationLifftoffAdapter" "7.4.2.1")
ADD_APPLOVIN_MEDIATION(LINE "AppLovinMediationLineAdapter" "2.9.20241106.0")
ADD_APPLOVIN_MEDIATION(MAIO "AppLovinMediationMaioAdapter" "2.1.5.0")
ADD_APPLOVIN_MEDIATION(META "AppLovinMediationFacebookAdapter" "6.16.0.0") #Meta
ADD_APPLOVIN_MEDIATION(MINTEGRAL "AppLovinMediationMintegralAdapter" "7.7.3.0.0")
ADD_APPLOVIN_MEDIATION(MOBILEFUSE "AppLovinMediationMobileFuseAdapter" "1.8.1.0")
ADD_APPLOVIN_MEDIATION(OGURY "AppLovinMediationOguryPresageAdapter" "5.0.1.0")
ADD_APPLOVIN_MEDIATION(PANGLE "AppLovinMediationByteDanceAdapter" "6.3.1.0.0") #Pangle
ADD_APPLOVIN_MEDIATION(SMAATO "AppLovinMediationSmaatoAdapter" "22.9.2.0")
ADD_APPLOVIN_MEDIATION(UNITYADS "AppLovinMediationUnityAdsAdapter" "4.12.5.0")
ADD_APPLOVIN_MEDIATION(VERVE "AppLovinMediationVerveAdapter" "3.1.3.0")
ADD_APPLOVIN_MEDIATION(MYTARGET "AppLovinMediationMyTargetAdapter" "5.22.0.0") #VK
ADD_APPLOVIN_MEDIATION(YANDEX "AppLovinMediationYandexAdapter" "7.5.0.3")

ADD_MENGINE_DOWNLOAD_SKADNETWORKITEMS_PLIST("https://skadnetwork-ids.applovin.com/v1/skadnetworkids.xml")

if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_UNITYADS)
    ADD_MENGINE_DOWNLOAD_SKADNETWORKITEMS_PLIST("https://skan.mz.unity3d.com/v2/partner/skadnetworks.plist.xml")
endif()

if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_UNITYADS)
	SET(URL_DOWNLOAD_UNITYADS_SKADNETWORKIDS "https://skan.mz.unity3d.com/v2/partner/skadnetworks.plist.xml")
	
	file(DOWNLOAD ${URL_DOWNLOAD_UNITYADS_SKADNETWORKIDS} ${MENGINE_SOLUTIONS_CONFIG_DIR}/downloads/Configuration/${PROJECT_NAME}/unityads_skadnetworkids.xml
	    STATUS STATUS_DOWNLOAD_UNITYADS_SKADNETWORKIDS
	)

	list(GET STATUS_DOWNLOAD_UNITYADS_SKADNETWORKIDS 0 STATUS_DOWNLOAD_UNITYADS_SKADNETWORKIDS_CODE)

	if(NOT STATUS_DOWNLOAD_UNITYADS_SKADNETWORKIDS_CODE EQUAL 0)
	    message(FATAL_ERROR "Failed to download file: ${URL_DOWNLOAD_UNITYADS_SKADNETWORKIDS}")
	else()
	    message(STATUS "File downloaded successfully: ${URL_DOWNLOAD_UNITYADS_SKADNETWORKIDS}")
	endif()
	
	ADD_MENGINE_PLIST(${MENGINE_SOLUTIONS_CONFIG_DIR}/downloads/Configuration/${PROJECT_NAME}/unityads_skadnetworkids.xml SKAdNetworkItems)
endif()

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
