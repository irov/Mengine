MENGINE_PROJECT(AppleAdvertisementPlugin)

ADD_FILTER(
src
    AppleAdvertisementPlugin.h
    AppleAdvertisementPlugin.mm
    
    AppleAdvertisementInterface.h
    
    AppleAdvertisementApplicationDelegate.h
    AppleAdvertisementApplicationDelegate.mm
    
    AppleAdvertisementAttempts.h
    AppleAdvertisementAttempts.mm
    
    AppleAdvertisementCooldown.h
    AppleAdvertisementCooldown.mm
    
    AppleAdvertisementBasePoint.h
    AppleAdvertisementBasePoint.mm
    
    AppleAdvertisementInterstitialPoint.h
    AppleAdvertisementInterstitialPoint.mm
    
    AppleAdvertisementRewardedPoint.h
    AppleAdvertisementRewardedPoint.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAdvertisementScriptEmbedding.h
        AppleAdvertisementScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_ADVERTISEMENT)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAdvertisementApplicationDelegate")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
