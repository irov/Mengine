MENGINE_PROJECT(AppleReviewPlugin)

ADD_FILTER(
src
    AppleReviewInterface.h
    AppleReviewService.h
    AppleReviewService.mm

    AppleReviewPlugin.h
    AppleReviewPlugin.cpp
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleReviewScriptEmbedding.h
        AppleReviewScriptEmbedding.cpp
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_REVIEW)

ADD_APPLE_FRAMEWORK(StoreKit)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()