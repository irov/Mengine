MENGINE_PROJECT_EXTRA_LANGUAGE(iOSStoreInAppPurchasePlugin Swift)

ADD_FILTER(
src
    iOSStoreInAppPurchaseInterface.h

    iOSStoreInAppPurchasePlugin.h
    iOSStoreInAppPurchasePlugin.mm
    iOSStoreInAppPurchaseSubscriptions.swift

	iOSStoreInAppPurchaseEntitlements.h
	iOSStoreInAppPurchaseEntitlements.mm

    iOSStoreInAppPurchasePaymentTransactionObserver.h
    iOSStoreInAppPurchasePaymentTransactionObserver.mm
    iOSStoreInAppPurchaseProductsRequestDelegate.h
    iOSStoreInAppPurchaseProductsRequestDelegate.mm

    iOSStoreInAppPurchasePaymentTransaction.h
    iOSStoreInAppPurchasePaymentTransaction.mm

    iOSStoreInAppPurchaseProduct.h
    iOSStoreInAppPurchaseProduct.mm
    iOSStoreInAppPurchaseProductsRequest.h
    iOSStoreInAppPurchaseProductsRequest.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSStoreInAppPurchaseScriptEmbedding.h
        iOSStoreInAppPurchaseScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_STOREINAPPPURCHASE)

set_target_properties(${PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_SWIFT_VERSION 5.0
    XCODE_ATTRIBUTE_SWIFT_OBJC_INTERFACE_HEADER_NAME "iOSStoreInAppPurchasePlugin-Swift.h"
)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSStoreInAppPurchasePlugin")

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
