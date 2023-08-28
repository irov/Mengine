MENGINE_PROJECT(AppleStoreReviewPlugin)

ADD_FILTER(
src
    AppleStoreReviewInterface.h
    AppleStoreReviewService.h
    AppleStoreReviewService.mm

    AppleStoreReviewPlugin.h
    AppleStoreReviewPlugin.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleStoreReviewScriptEmbedding.h
        AppleStoreReviewScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_STOREREVIEW)

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()