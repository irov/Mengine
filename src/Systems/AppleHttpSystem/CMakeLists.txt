MENGINE_PROJECT(AppleHttpSystem)

ADD_FILTER(
src
    AppleHttpSystem.mm
    AppleHttpSystem.h

    AppleHttpNetwork.mm
    AppleHttpNetwork.h
    
    AppleHttpRequest.mm
    AppleHttpRequest.h
    AppleHttpRequestGetAsset.mm
    AppleHttpRequestGetAsset.h
    AppleHttpRequestGetMessage.mm
    AppleHttpRequestGetMessage.h
    AppleHttpRequestHeaderData.mm
    AppleHttpRequestHeaderData.h
    AppleHttpRequestPostMessage.mm
    AppleHttpRequestPostMessage.h
    AppleHttpRequestDeleteMessage.mm
    AppleHttpRequestDeleteMessage.h
)

ADD_MENGINE_APPLE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)