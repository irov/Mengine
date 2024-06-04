MENGINE_PROJECT(AndroidHttpSystem)

ADD_FILTER(
src
    AndroidHttpSystem.cpp
    AndroidHttpSystem.h

    AndroidHttpRequest.cpp
    AndroidHttpRequest.h
    AndroidHttpRequestGetAsset.cpp
    AndroidHttpRequestGetAsset.h
    AndroidHttpRequestGetMessage.cpp
    AndroidHttpRequestGetMessage.h
    AndroidHttpRequestHeaderData.cpp
    AndroidHttpRequestHeaderData.h
    AndroidHttpRequestPostMessage.cpp
    AndroidHttpRequestPostMessage.h
    AndroidHttpRequestDeleteMessage.cpp
    AndroidHttpRequestDeleteMessage.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)