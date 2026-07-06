MENGINE_PROJECT(iOSStoreReviewPlugin)

ADD_FILTER(
src
    iOSStoreReviewInterface.h

    iOSStoreReviewPlugin.h
    iOSStoreReviewPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSStoreReviewScriptEmbedding.h
        iOSStoreReviewScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_STOREREVIEW)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSStoreReviewPlugin")

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
