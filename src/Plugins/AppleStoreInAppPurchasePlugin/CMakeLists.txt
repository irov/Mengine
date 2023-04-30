MENGINE_PROJECT(AppleStoreInAppPurchasePlugin)

ADD_FILTER(
src
    AppleStoreInAppPurchaseInterface.h
    AppleStoreInAppPurchaseFactoryInterface.h

    AppleStoreInAppPurchasePlugin.h
    AppleStoreInAppPurchasePlugin.mm
    
    AppleStoreInAppPurchaseService.h
    AppleStoreInAppPurchaseService.mm

    AppleStoreInAppPurchasePaymentQueueDelegate.h
    AppleStoreInAppPurchasePaymentQueueDelegate.mm

    AppleStoreInAppPurchasePaymentTransactionObserver.h
    AppleStoreInAppPurchasePaymentTransactionObserver.mm
    AppleStoreInAppPurchaseProductsRequestDelegate.h
    AppleStoreInAppPurchaseProductsRequestDelegate.mm

    AppleStoreInAppPurchasePaymentTransaction.h
    AppleStoreInAppPurchasePaymentTransaction.mm

    AppleStoreInAppPurchaseProduct.h
    AppleStoreInAppPurchaseProduct.mm
    AppleStoreInAppPurchaseProductsRequest.h
    AppleStoreInAppPurchaseProductsRequest.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleStoreInAppPurchaseScriptEmbedding.h
        AppleStoreInAppPurchaseScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_STOREINAPPPURCHASE)

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()