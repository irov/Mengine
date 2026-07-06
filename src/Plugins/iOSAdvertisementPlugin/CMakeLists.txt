MENGINE_PROJECT(iOSAdvertisementPlugin)

ADD_FILTER(
src
    iOSAdvertisementInterface.h

    iOSAdvertisementPlugin.h
    iOSAdvertisementPlugin.mm

    iOSAdvertisementAttempts.h
    iOSAdvertisementAttempts.mm

    iOSAdvertisementCooldown.h
    iOSAdvertisementCooldown.mm

    iOSAdvertisementBasePoint.h
    iOSAdvertisementBasePoint.mm

    iOSAdvertisementInterstitialPoint.h
    iOSAdvertisementInterstitialPoint.mm

    iOSAdvertisementRewardedPoint.h
    iOSAdvertisementRewardedPoint.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSAdvertisementScriptEmbedding.h
        iOSAdvertisementScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_ADVERTISEMENT)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSAdvertisementPlugin")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
