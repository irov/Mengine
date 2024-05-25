MENGINE_PROJECT(AppleHttpSystem)

ADD_FILTER(
src
    AppleHttpSystem.mm
    AppleHttpSystem.h
    
    AppleHttpResponse.mm
    AppleHttpResponse.h
    
    AppleHttpRequestThreadTask.mm
    AppleHttpRequestThreadTask.h
    AppleHttpRequestGetAssetThreadTask.mm
    AppleHttpRequestGetAssetThreadTask.h
    AppleHttpRequestGetMessageThreadTask.mm
    AppleHttpRequestGetMessageThreadTask.h
    AppleHttpRequestHeaderDataThreadTask.mm
    AppleHttpRequestHeaderDataThreadTask.h
    AppleHttpRequestPostMessageThreadTask.mm
    AppleHttpRequestPostMessageThreadTask.h
    AppleHttpRequestDeleteMessageThreadTask.mm
    AppleHttpRequestDeleteMessageThreadTask.h
)

ADD_MENGINE_APPLE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)