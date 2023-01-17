MENGINE_PROJECT(AppleStoreInAppPurchasePlugin)

ADD_FILTER(
src
    AppleStoreInAppPurchasePlugin.h
    AppleStoreInAppPurchasePlugin.cpp
    
    AppleStoreInAppPurchaseInterface.h
    AppleStoreInAppPurchaseService.h
    AppleStoreInAppPurchaseService.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleStoreInAppPurchaseScriptEmbedding.h
        AppleStoreInAppPurchaseScriptEmbedding.cpp
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_STOREINAPPPURCHASE)

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()