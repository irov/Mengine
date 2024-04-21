MENGINE_PROJECT(AppleAdjustPlugin)

ADD_FILTER(
src
    AppleAdjustPlugin.h
    AppleAdjustPlugin.mm

    AppleAdjustInterface.h

    AppleAdjustService.h
    AppleAdjustService.mm

    AppleAdjustApplicationDelegate.h
    AppleAdjustApplicationDelegate.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAdjustScriptEmbedding.h
        AppleAdjustScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_ADJUST)

ADD_MENGINE_COCOAPOD("Adjust" "NO-GIT" "4.38.1")
ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAdjustApplicationDelegate")

IF(MENGINE_TARGET_IOS)
    ADD_MENGINE_APPLE_FRAMEWORK(AdSupport)
    ADD_MENGINE_APPLE_FRAMEWORK(iAd)
    ADD_MENGINE_APPLE_FRAMEWORK(AdServices)
    ADD_MENGINE_APPLE_FRAMEWORK(CoreTelephony)
    ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)
    ADD_MENGINE_APPLE_FRAMEWORK(AppTrackingTransparency)
    ADD_MENGINE_APPLE_FRAMEWORK(WebKit)
ENDIF()

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
