MENGINE_PROJECT(AppleStoreInAppPurchasePlugin)

ADD_FILTER(
src
    AppleStoreInAppPurchaseInterface.h
    AppleStoreInAppPurchaseFactoryInterface.h

    AppleStoreInAppPurchasePlugin.h
    AppleStoreInAppPurchasePlugin.mm
    
    AppleStoreInAppPurchaseService.h
    AppleStoreInAppPurchaseService.mm
    
    AppleStoreInAppPurchaseApplicationDelegate.h
    AppleStoreInAppPurchaseApplicationDelegate.mm

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

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AppleStoreInAppPurchaseScriptEmbedding.h
        AppleStoreInAppPurchaseScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_STOREINAPPPURCHASE)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleStoreInAppPurchaseApplicationDelegate")

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
