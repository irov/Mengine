MENGINE_PROJECT(AppodealPlugin)

ADD_FILTER(
src
    # SentryInterface.h

    # SentryPlugin.h
    # SentryPlugin.def
    # SentryPlugin.cpp
)


if(MENGINE_TARGET_APPLE)
    ADD_FILTER(
    apple
        AppodealImplementation.h
        AppodealImplementation.m
    
    )
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPODEAL)

if(MENGINE_TARGET_APPLE)
    ADD_MENGINE_COCOAPOD("APDAdColonyAdapter" "2.11.1.3")
    ADD_MENGINE_COCOAPOD("APDAmazonAdsAdapter" "2.11.1.2")
    ADD_MENGINE_COCOAPOD("APDAppLovinAdapter" "2.11.1.3")
    ADD_MENGINE_COCOAPOD("APDBidMachineAdapter" "2.11.1.3") # Required
    #ADD_MENGINE_COCOAPOD("APDFacebookAudienceAdapter" "2.11.1.2")
    ADD_MENGINE_COCOAPOD("APDGoogleAdMobAdapter" "2.11.1.3")
    ADD_MENGINE_COCOAPOD("APDIronSourceAdapter" "2.11.1.3")
    ADD_MENGINE_COCOAPOD("APDMyTargetAdapter" "2.11.1.2")
    ADD_MENGINE_COCOAPOD("APDOguryAdapter" "2.11.1.1")
    ADD_MENGINE_COCOAPOD("APDUnityAdapter" "2.11.1.2")
    ADD_MENGINE_COCOAPOD("APDVungleAdapter" "2.11.1.2")
    ADD_MENGINE_COCOAPOD("APDYandexAdapter" "2.11.1.1")
endif()