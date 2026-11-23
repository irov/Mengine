MENGINE_PROJECT_EXTRA_LANGUAGE(iOSStoreKitPlugin Swift)

ADD_FILTER(
src
    iOSStoreKitInterface.h

    iOSStoreKitPlugin.h
    iOSStoreKitPlugin.mm

    iOSStoreKitService.swift
    iOSStoreKitServiceDelegate.swift

    iOSStoreKitEntitlements.h
    iOSStoreKitEntitlements.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSStoreKitScriptEmbedding.h
        iOSStoreKitScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_STOREKIT)

set_target_properties(${PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_SWIFT_VERSION 5.0
    XCODE_ATTRIBUTE_SWIFT_OBJC_INTERFACE_HEADER_NAME "iOSStoreKitPlugin-Swift.h"
)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSStoreKitPlugin")

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} SwiftEnvironment)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PythonEnvironment)
endif()
